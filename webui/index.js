
$ = document.querySelector.bind(document)
$$ = document.querySelectorAll.bind(document)

/**
 * Imports a javascript file
 *
 * @param jsFile The name of the javascript file to import
 */
function require(jsFile) {
    let el = document.createElement('script');
    el.setAttribute('src', jsFile);
    document.head.append(el);
    return el;
}

/**
 * Determines if we are running in electron
 */
function isElectron() {
    return /electron/i.test(navigator.userAgent);
}

require('interface.js');
require('dwrandomizer.js');
require('base32.js');

let rom; let sprite_choices = [];
let log = ''
let error = ''

/**
 * Handler for wasm stdout
 */
function stdout(asciiCode) {
    new_char = String.fromCharCode(asciiCode);
    log += new_char
    if (new_char.endsWith('\n')) {
        console.log(log);
        log = '';
    }
}

/**
 * Handler for wasm stderr.
 */
function stderr(asciiCode) {
    new_char = String.fromCharCode(asciiCode);
    error += new_char
    if (new_char.endsWith('\n')) {
        console.error(error);
        error = '';
    }
}

/**
 * A class for holding ROM data
 */
class Rom extends Uint8Array {
    header() {
        return this.slice(0, 16);
    }

    /**
     * Overwrites data in the ROM
     *
     * @param addr The address to overwrite (not including the header)
     * @param data The new data
     */
    set(addr, data) {
        for (let i = 0; i < data.length; i++) {
            // add 16 to skip the header
            this[addr + 16 + i] = data[i];
        }
    }

    /**
     * Calls the wasm randomize() function with the provided arguments
     *
     * @param seed The seed number (64-bit).
     * @param flags The flags string (base32-encoded).
     * @param sprite The player's chosen sprite.
     */
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

    /**
     * Prompts the user to save the generated ROM file
     */
    save() {
        this.output = FS.readFile('/'+this.outname);
        FS.unlink('/'+this.outname);
        let url = URL.createObjectURL(new Blob([this.output]), {
            type: 'application/octet-stream'
        });
        let downloader = document.createElement('a');
        downloader.href = url;
        downloader.download = this.outname;
        downloader.click();
        URL.revokeObjectURL(url);
    }
}

/**
 * Initializes the user interface
 */
function setup_ui() {
    if (!localStorage.flags)
        localStorage.flags = 'IVIAAVCAKACAAAAAAAAAAEAA'
    if (!localStorage.retainFlags)
        localStorage.retainFlags = 'AAAAAAAAAAAAAAAAAAAAAAAA'
    ui = new Interface(15);
    ui.addTab('Gameplay');
    ui.addTab('Features');
    ui.addTab('Monsters');
    ui.addTab('Shortcuts');
    ui.addTab('Challenge');
    ui.addTab('Cosmetic');
    ui.addSummaryTab('Summary');
    ui.setActiveTab('Gameplay');

    ui.addTriOption('Gameplay',  0,  0, 6, 'Shuffle Chests & Searches',
        'The items in chests and search locations will be randomized.');
    ui.addTriOption('Gameplay',  2,  0, 4, 'Random Chest Locations',
        'Chests will be at a random set of predetermined locations.');
    ui.addTriOption('Gameplay',  4,  0, 2, 'Random Growth',
        'Player statistical growth at each level will be randomized.');
    ui.addTriOption('Gameplay',  6,  0, 0, 'Random Map',
        'The overworld map will be randomly generated.');
    ui.addTriOption('Gameplay',  8,  1, 6, 'Random Spell Learning',
        'The order and level you learn spells will be random.');
    ui.addTriOption('Gameplay',  1,  1, 4, 'Random Weapon Shops',
        'The weapons available in each shop will be randomized.');
    ui.addTriOption('Gameplay',  3,  1, 2, 'Random Weapon Prices',
        'Pretty self-explanatory.');
    ui.addTriOption('Gameplay',  5,  1, 0, 'Random XP Requirements',
        'Experience requirements for each level will be randomized.');
    ui.addTriOption('Gameplay',  7,  2, 6, 'Heal/Hurt Before "More"',
        'HEAL must come before HEALMORE; HURT before HURTMORE.');
    ui.addTriOption('Gameplay', 10,  2, 2, 'Stair Shuffle',
        'Where stairs take you inside dungeons will be randomized.');

    ui.addTriOption('Features',  0,  3, 6, 'Enable Menu Wrapping',
        'This enables cursor wrapping in menus.');
    ui.addTriOption('Features',  2,  3, 4, 'Enable Death Necklace',
        'Equipping the death necklace will cause +10AP and -25% HP.');
    ui.addTriOption('Features',  4,  3, 2, 'Enable Torches In Battle',
        'Torches and Fairy water can be thrown at monsters.');
    ui.addTriOption('Features',  1,  4, 6, 'Repel in Dungeons',
        'Enables REPEL to work in dungeons');
    ui.addTriOption('Features',  3,  4, 4, 'Permanent Repel',
        'REPEL will always be active.');
    ui.addTriOption('Features',  5,  4, 2, 'Permanent Torch',
        'At least a 3x3 area will always be lit in dungeons.');
    ui.addTriOption('Features',  6,  4, 0, 'Alternate Running Algorithm',
        'The run blocking multiplier will depend on where you are.');
    ui.addTriOption('Monsters',  0,  5, 6, 'Random Monster Abilities',
        'Monster spells and abilities will be randomized.');
    ui.addTriOption('Monsters',  2,  5, 4, 'Random Monster Zones',
        'Monster encounters in each zone will be randomized.');
    ui.addTriOption('Monsters',  4,  5, 2, 'Random Monster Stats',
        'Monster strength, agility, and HP will be randomized.');
    ui.addTriOption('Monsters',  6,  5, 0, 'Random Monster XP & Gold',
        'The XP and GOLD gained from monsters will be randomized.');
    ui.addTriOption('Monsters',  8,  6, 6, 'Make Random Stats Consistent',
        'This makes the random stats, XP, and GOLD consistent with one another.');
    ui.addTriOption('Monsters',  1,  6, 4, 'Scared Metal Slimes',
        'Metal Slimes will always have a chance to run.');
    ui.addTriOption('Monsters',  3,  6, 2, 'Scaled Metal Slime XP',
        'Metal Slime XP will depend on your current level.');

    ui.addTriOption('Shortcuts',  0,  7, 6, 'Fast Text',
        'All text will progress much faster.');
    ui.addTriOption('Shortcuts',  2,  7, 4, 'Speed Hacks',
        'Various aspects of the game will be much faster.');
    ui.addTriOption('Shortcuts',  4,  7, 2, 'Open Charlock',
        'No need to create a bridge to enter Charlock Castle.');
    ui.addTriOption('Shortcuts',  6,  7, 0, 'Short Charlock',
        'Charlock Dungeon will be much shorter.');
    ui.addTriOption('Shortcuts',  8,  8, 6, "Don't Require Magic Keys",
        'All of the doors are unlocked. Just open them.');
    ui.addTriOption('Shortcuts', 10, 13, 2, "Summer Sale",
        'All weapons and armor 35-65% off!');
    ui.addTriOption('Shortcuts', 12, 15, 0, 'Medium Text',
        'Various aspects of the game will be much faster.');
    ui.addTriOption('Shortcuts', 14, 15, 4, 'Double Walking',
        'Various aspects of the game will be much faster.');
    ui.addTriOption('Shortcuts',  1,  8, 2, 'Cursed Princess',
        'Get Gwaelin to take a Cursed Belt when you return her to win.');
    ui.addTriOption('Shortcuts',  3,  8, 0, "Three's Company",
        'Bring Gwaelin to the Dragonlord and accept his offer to win.');
    // leveling speed
    ui.addDropDown ('Shortcuts',  7, 13, 4, 'Leveling Speed', {
        'Normal' : 0,
        'Fast' : 1,
        'Very Fast': 2
    });
    ui.addDropDown ('Shortcuts',  9, 13, 6, 'Random Map Size', {
        'Normal' : 0,
        'Small' : 1,
        'Very Small': 2,
        'Random': 3
    });

    ui.addTriOption('Challenge',  0,  9, 6, 'No Hurtmore',
        'You will never learn HURTMORE. Monsters will still have it.');
    ui.addTriOption('Challenge',  2,  9, 4, 'No Numbers',
        'No numbers will be visible until the Dragonlord fight.');
    ui.addTriOption('Challenge',  4,  9, 2, 'Invisible Hero',
        'Your sprite will be invisible.');
    ui.addTriOption('Challenge',  6,  9, 0, 'Invisible NPCs',
        'All NPCs will be invisible.');
    ui.addTriOption('Challenge',  1, 13, 0, 'Treasure Guards',
        'Important items will have a mid-level monster guarding them.');
    ui.addTriOption('Challenge',  3, 10, 0, 'Big Swamp',
        'Approximately 60% of the overworld will be poisonous swamp.');
    ui.addTriOption('Challenge',  5, 10, 2, 'Randomly Rotate/Mirror Dungeons',
        'All dungeons will be rotated at random angles and/or mirrored.');

    ui.addTriOption('Cosmetic',  4, 11, 6, 'Modern Spell Names',
        'Use spell names from more recent DQ releases.');
    ui.addTriOption('Cosmetic',  6, 11, 4, 'Noir Mode',
        "It's all black and white baby!");
    ui.addOption   ('Cosmetic',  0, 14, 7, 'Shuffle Music',
        'Music in each area will be randomized.');
    ui.addOption   ('Cosmetic',  2, 14, 6, 'Disable Music',
        'This disables the game music in most situations.');
    ui.addOption   ('Cosmetic',  8, 14, 5, 'Disable Spell Flashing',
        'Prevents the screen from flashing when you cast spells.', true);
    ui.addOption   ('Cosmetic',  10, 14, 4, 'Show Death Counter',
        'The stats window will also have a death counter.', true);
    ui.addOption   ('Cosmetic',   3, 14, 3, 'Allow Custom Spell Names',
        'Allow spell names to be changed based on the chosen sprite.', true);
    ui.addOption   ('Cosmetic',   5, 14, 2, 'Skip Original Credits',
        'Skip the original credits and go straight to stat scroll.', true);

    // player sprite
    let spriteBox;
    if (!isElectron()) {
        spriteBox = ui.addTextBox('Cosmetic', 1, 'Player Sprite');
        spriteBox.setAttribute('list', 'sprites');
        spriteBox.value = localStorage.getItem('sprite') || 'Random';
        spriteBox.addEventListener('focus', function(event) {
            this.value = '';
        });
    } else {
        spriteBox = ui.addDropDown('Cosmetic', 1, null, null, 
            'Player Sprite', null, true);
        spriteBox.style.marginRight = '0.2em';
        spriteBox.style.width = '190px';
        spriteBox.parentElement.style.marginRight = "0";
    }
    spriteBox.getValue = function() {
        if (!sprite_choices.includes(this.value))
            return 'Random';
        return this.value;
    }
    spriteBox.id = "sprite-box";
    spriteBox.addEventListener(isElectron() ? 'change' : 'focusout',
        function(event) {
            if (this.value == '') {
                this.value = localStorage.getItem('sprite') || 'Random';
            }
            if (sprite_choices.includes(this.value)) {
                this.classList.remove('invalid');
                localStorage.setItem('sprite', this.value);
                spritePreview.setAttribute('src', 'sprites/' + this.getValue() 
                    + '.png');
            } else {
                this.classList.add('invalid');
            }
            ui.updateSummary();
    });
    let spritePreview = ui.create('img', null, {
        'background-color': '#ccc',
        'border': '2px solid #ccc',
        'float': 'right',
        'height': '32px',
        'width': '32px'
    })
    spritePreview.id = 'sprite-preview';
    spritePreview.setAttribute('src', 'sprites/' 
        + (localStorage.getItem('sprite') || 'Random') + '.png');
    spriteBox.parentElement.append(spritePreview);
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
        let spriteBox = $('#sprite-box');
        let i=0;
        while(true) {
            let name = sprite_name(i++);
            if (!name) break;
            let option = document.createElement('option');
            option.value = option.innerText = name;
            sprite_choices.push(name);
            if (isElectron()) {
                spriteBox.append(option);
            } else {
                sprites_datalist.append(option);
            }
        }
        if (isElectron())
            spriteBox.value = localStorage.getItem('sprite') || 'Random';
    };

    let rom_data = localStorage.getItem('rom_data');
    if (rom_data) {
        rom = new Rom(rom_data.split(','));
        ui.setInputFile(localStorage.getItem('rom_name'));
    }
});

