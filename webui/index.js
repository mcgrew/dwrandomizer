
$ = document.querySelector.bind(document)
$$ = document.querySelectorAll.bind(document)

function require(jsFile) {
    let el = document.createElement('script');
    el.setAttribute('src', jsFile);
    document.head.append(el);
    return el;
}


require('interface.js');
require('dwrandomizer.js');
require('base32.js');

let rom;
let sprite_choices = [];
let log = ''
let error = ''
function stdout(asciiCode) {
    new_char = String.fromCharCode(asciiCode);
    log += new_char
    if (new_char.endsWith('\n')) {
        console.log(log);
        log = '';
    }
}

function stderr(asciiCode) {
    new_char = String.fromCharCode(asciiCode);
    error += new_char
    if (new_char.endsWith('\n')) {
        console.error(error);
        error = '';
    }
}

class Rom extends Uint8Array {
    header() {
        return this.slice(0, 16);
    }

    set(addr, data) {
        for (let i = 0; i < data.length; i++) {
            // add 16 to skip the header
            this[addr + 16 + i] = data[i];
        }
    }

    randomize(seed, flags, sprite) {
        console.log("Randomizing...");
        FS.writeFile('/'+this.name, this);

        this.seed = seed;
        this.flags = flags;
        let checksum = Module.ccall('dwr_randomize', 'BigInt',
            ['string', 'BigInt', 'string', 'string', 'string'],
            ['/'+this.name, seed, flags, sprite, '/'])
        this.outname = 'DWRando.' + seed + '.' + flags + '.nes';
        // return value is signed, so fix it if it's negative
        if (checksum < 0) checksum += 18446744073709551616n;
        return checksum;
    }

    save() {
        this.output = FS.readFile('/'+this.outname, null);
        FS.unlink('/'+this.outname);
        let url = URL.createObjectURL(new Blob([this.output]), {
            type: 'application/octet-stream'
        });
        let downloader = document.createElement('a');
        downloader.href = url;
        downloader.download = this.outname;
        downloader.click()
    }
}

function setup_ui() {
    ui = new Interface(15);
    ui.addTab('Gameplay');
    ui.addTab('Features');
    ui.addTab('Monsters');
    ui.addTab('Shortcuts');
    ui.addTab('Challenge');
    ui.addTab('Cosmetic');
    ui.addSummaryTab('Summary');
    ui.setActiveTab('Gameplay');

    ui.addTriOption('Gameplay',  0,  0, 6, 'Shuffle Chests & Searches');
    ui.addTriOption('Gameplay',  2,  0, 4, 'Random Chest Locations');
    ui.addTriOption('Gameplay',  4,  0, 2, 'Random Growth');
    ui.addTriOption('Gameplay',  6,  0, 0, 'Random Map');
    ui.addTriOption('Gameplay',  8,  1, 6, 'Random Spell Learning');
    ui.addTriOption('Gameplay',  1,  1, 4, 'Random Weapon Shops');
    ui.addTriOption('Gameplay',  3,  1, 2, 'Random Weapon Prices');
    ui.addTriOption('Gameplay',  5,  1, 0, 'Random XP Requirements');
    ui.addTriOption('Gameplay',  7,  2, 6, 'Heal/Hurt Before "More"');
    ui.addTriOption('Gameplay',  9,  2, 4, 'Randomly Rotate/Mirror Dungeons');

    ui.addTriOption('Features',  0,  3, 6, 'Enable Menu Wrapping');
    ui.addTriOption('Features',  2,  3, 4, 'Enable Death Necklace');
    ui.addTriOption('Features',  4,  3, 2, 'Enable Torches In Battle');
    ui.addTriOption('Features',  6,  3, 0, 'Big Swamp');
    ui.addTriOption('Features',  1,  4, 6, 'Repel in Dungeons');
    ui.addTriOption('Features',  3,  4, 4, 'Permanent Repel');
    ui.addTriOption('Features',  5,  4, 2, 'Permanent Torch');
    ui.addTriOption('Features',  8,  4, 0, 'Alternate Running Algorithm');

    ui.addTriOption('Monsters',  0,  5, 6, 'Random Monster Abilities');
    ui.addTriOption('Monsters',  2,  5, 4, 'Random Monster Zones');
    ui.addTriOption('Monsters',  4,  5, 2, 'Random Monster Stats');
    ui.addTriOption('Monsters',  6,  5, 0, 'Random Monster XP & Gold');
    ui.addTriOption('Monsters',  8,  6, 6, 'Make Random Stats Consistent');
    ui.addTriOption('Monsters',  1,  6, 4, 'Scared Metal Slimes');
    ui.addTriOption('Monsters',  3,  6, 2, 'Scaled Metal Slime XP');

    ui.addTriOption('Shortcuts', 0,  7, 6, 'Fast Text');
    ui.addTriOption('Shortcuts', 2,  7, 4, 'Speed Hacks');
    ui.addTriOption('Shortcuts', 4,  7, 2, 'Open Charlock');
    ui.addTriOption('Shortcuts', 6,  7, 0, 'Short Charlock');
    ui.addTriOption('Shortcuts', 8,  8, 6, "Don't Require Magic Keys");
    ui.addTriOption('Shortcuts', 1,  8, 2, 'Cursed Princess');
    ui.addTriOption('Shortcuts', 3,  8, 0, "Three's Company");
    // leveling speed
    ui.addDropDown ('Shortcuts', 7, 14, 0, 'Leveling Speed', {
        'Normal' : 0,
        'Fast' : 1,
        'Very Fast': 2
    });
    ui.addDropDown ('Shortcuts', 9, 14, 2, 'Map Size', {
        'Normal' : 0,
        'Small' : 1,
        'Very Small': 2,
        'Random': 3
    });

    ui.addTriOption('Challenge', 0,  9, 6, 'No Hurtmore');
    ui.addTriOption('Challenge', 2,  9, 4, 'No Numbers');
    ui.addTriOption('Challenge', 4,  9, 2, 'Invisible Hero');
    ui.addTriOption('Challenge', 6,  9, 0, 'Invisible NPCs');

    ui.addTriOption('Cosmetic',  4, 10, 6, 'Modern Spell Names');
    ui.addTriOption('Cosmetic',  6, 10, 4, 'Noir Mode');
    ui.addOption   ('Cosmetic',  0, 14, 7, 'Shuffle Music');
    ui.addOption   ('Cosmetic',  2, 14, 6, 'Disable Music');
    ui.addOption   ('Cosmetic',  8, 14, 5, 'Disable Spell Flashing');
    ui.addOption   ('Cosmetic',  3, 13, 7, 'Allow Custom Spell Names');

    // player sprite
    let spriteBox = ui.addTextBox('Cosmetic', 1, 'Player Sprite: ');
    spriteBox.setAttribute('list', 'sprites');
    spriteBox.value = localStorage.getItem('sprite') || 'Random';
    spriteBox.id = "sprite-box";
    spriteBox.getValue = function() {
        if (this.classList.contains('invalid'))
            return 'Random';
        return this.value;
    }
    spriteBox.addEventListener('change', function(event) {
        if (sprite_choices.includes(this.value)) {
            this.classList.remove('invalid');
            localStorage.setItem('sprite', this.value);
        } else {
            this.classList.add('invalid');
        }
        ui.updateSummary();
    });
    ui.updateSummary();
}

window.addEventListener('load', event => {
    Module["noFSInit"] = true;
    FS.init(() => {}, stdout, stdout);

    setup_ui();
    Module.onRuntimeInitialized = () => {
        ui.setVersion(Module.ccall('version', 'string'));
        let sprite_name = Module.cwrap('sprite_name', 'string', ['number']);
        let sprites_datalist = $('datalist#sprites');
        let i=0;
        while(true) {
            let name = sprite_name(i++);
            if (!name) break;
            let option = document.createElement('option');
            option.value = option.innerText = name;
            sprite_choices.push(name);
            sprites_datalist.append(option);
        }
    };

    let rom_data = localStorage.getItem('rom_data');
    if (rom_data) {
        rom = new Rom(rom_data.split(','));
        ui.setInputFile(localStorage.getItem('rom_name'));
    }
});

