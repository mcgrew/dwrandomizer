
$ = document.querySelector.bind(document)
$$ = document.querySelectorAll.bind(document)

NodeList.prototype.addEventListener = function(evt_type, func) {
    this.forEach(function(node) {
        node.addEventListener(evt_type, func.bind(node));
    });
};

NodeList.prototype.hide = function() {
    this.forEach(function(node) {
        node.hide()
    });
}

HTMLElement.prototype.hide = function() {
    this.style.display = 'none';
}

HTMLElement.prototype.show = function() {
    this.style.display = '';
}

HTMLInputElement.prototype.triState = function() {
    this.addEventListener('change', function(event) {
        if (!this.checked) {
            this.indeterminate = true;
            this.dataset.indeterminate = true;
        } else if (this.dataset.indeterminate) {
            delete this.dataset.indeterminate;
            this.checked = false;
        }
    });
}

Module["noFSInit"] = true;

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

FS.init(() => {}, stdout, stdout);

String.prototype.basename = function() {
    let index = this.lastIndexOf('/');
    if (index < 0)
        index = this.lastIndexOf('\\');
    return this.substring(index + 1);
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
        Module.ccall('dwr_randomize', 'BigInt',
            ['string', 'BigInt', 'string', 'string', 'string'],
            ['/'+this.name, seed, flags, sprite, '/']);
        this.outname = 'DWRando.' + seed + '.' + flags + '.nes';
    }

    save() {
        this.output = FS.readFile('/'+this.outname, null);
        let url = URL.createObjectURL(new Blob([this.output]), {
            type: 'application/octet-stream'
        });
        let downloader = document.createElement('a');
        downloader.href = url;
        downloader.download = this.outname;
        downloader.dispatchEvent(new MouseEvent('click'));
    }
}

class Interface {
    constructor() {
        this.tabBar = $('tabbar')
        this.tabContainer = $('tabcontainer')
        this.tabs = {};
    }
    
    addTab(name) {
        let tab = document.createElement('tab');
        tab.innerText = name;
        tab.id = name + '-tab';
        tab.addEventListener('click', function(event) {
            ui.setActiveTab(this.innerText);
        });
        this.tabBar.append(tab);
        let content = document.createElement('tabcontent');
        content.id = name + '-flags'
        for (let i=0; i < 10; i++) {
            content.append(document.createElement('flagcontainer'))
        }
        this.tabContainer.append(content);
        this.tabs[name] = {
            'tab': tab,
            'content': content
        }
    }

    setVersion(version) {
        // do something here.
    }

    setInputFile(name) {
        // do something here.
    }

    setActiveTab(name) {
        $$('tab').forEach(function(tab) { tab.classList.remove('active') });
        $$('tabcontent').hide();
        if (this.tabs[name]) {
            this.tabs[name].tab.classList.add('active');
            this.tabs[name].content.show();
        }

    }

    addTextBox(tab, position, title) {
        let input = document.createElement('input')
        input.name = tab + position;
        let label = document.createElement('label')
        label.for = tab + position;
        label.innerText = title;
        let container = this.tabs[tab].content.children[position];
        container.innerHTML = '';
        container.append(label);
        container.append(input);
        return input;
    }

    addDropDown(tab, position, values) {
    }

    addOption(tab, position, bitpos, title) {
        let input = document.createElement('input')
        input.type = 'checkbox';
        input.name = tab + position;
        let label = document.createElement('label')
        label.for = tab + position;
        label.innerText = title;
        let container = this.tabs[tab].content.children[position];
        container.innerHTML = '';
        label.prepend(input);
        container.append(label);
        return input;
    }

    addTriOption(tab, position, bitpos, title) {
        let input = document.createElement('input')
        input.type = 'checkbox';
        input.name = tab + position;
        input.triState();
        let label = document.createElement('label')
        label.for = tab + position;
        label.innerText = title;
        let container = this.tabs[tab].content.children[position];
        container.innerHTML = '';
        label.prepend(input);
        container.append(label);
        return input;
    }
}

function setup_ui() {
    ui = new Interface();
    ui.addTab('Gameplay');
    ui.addTab('Features');
    ui.addTab('Monsters');
    ui.addTab('Shortcuts');
    ui.addTab('Challenge');
    ui.addTab('Goals');
    ui.addTab('Cosmetic');
    ui.setActiveTab('Gameplay');

    ui.addTriOption('Gameplay', 0, null, 'Shuffle Chests & Searches');
    ui.addTriOption('Gameplay', 2, null, 'Random Growth');
    ui.addTriOption('Gameplay', 4, null, 'Random Map');
    ui.addTriOption('Gameplay', 6, null, 'Random Spell Learning');
    ui.addTriOption('Gameplay', 8, null, 'Heal/Hurt Before "More"');
    ui.addTriOption('Gameplay', 1, null, 'Random Weapon Shops');
    ui.addTriOption('Gameplay', 3, null, 'Random Weapon Prices');
    ui.addTriOption('Gameplay', 5, null, 'Random XP Requirements');

    ui.addTriOption('Features', 0, null, 'Enable Menu Wrapping');
    ui.addTriOption('Features', 2, null, 'Enable Death Necklace');
    ui.addTriOption('Features', 4, null, 'Enable Torches In Battle');
    ui.addTriOption('Features', 1, null, 'Big Swamp');
    ui.addTriOption('Features', 3, null, 'Repel in Dungeons');
    ui.addTriOption('Features', 5, null, 'Permanent Repel');
    ui.addTriOption('Features', 7, null, 'Permanent Torch');

    ui.addTriOption('Monsters', 0, null, 'Random Monster Abilities');
    ui.addTriOption('Monsters', 2, null, 'Random Monster Zones');
    ui.addTriOption('Monsters', 4, null, 'Random Monster Stats');
    ui.addTriOption('Monsters', 6, null, 'Random Monster XP & Gold');
    ui.addTriOption('Monsters', 8, null, 'Make Random Stats Consistent');
    ui.addTriOption('Monsters', 1, null, 'Scared Metal Slimes');
    ui.addTriOption('Monsters', 3, null, 'Scaled Metal Slime XP');

    ui.addTriOption('Shortcuts', 0, null, 'Fast Text');
    ui.addTriOption('Shortcuts', 2, null, 'Speed Hacks');
    ui.addTriOption('Shortcuts', 4, null, 'Open Charlock');
    ui.addTriOption('Shortcuts', 6, null, 'Short Charlock');
    ui.addTriOption('Shortcuts', 8, null, "Don't Require Magic Keys");
    // leveling speed
    
    ui.addTriOption('Challenge', 0, null, 'No Hurtmore');
    ui.addTriOption('Challenge', 2, null, 'No Numbers');
    ui.addTriOption('Challenge', 4, null, 'Invisible Hero');
    ui.addTriOption('Challenge', 6, null, 'Invisible NPCs');

    ui.addTriOption('Goals', 0, null, 'Cursed Princess');
    ui.addTriOption('Goals', 2, null, "Three's Company");

    ui.addOption('Cosmetic', 0, null, 'Shuffle Music');
    ui.addOption('Cosmetic', 2, null, 'Disable Music');
    ui.addOption('Cosmetic', 4, null, 'Modern Spell Names');
    ui.addOption('Cosmetic', 6, null, 'Noir Mode');
    ui.addOption('Cosmetic', 8, null, 'Disable Spell Flashing');

    // player sprite
    spriteBox = ui.addTextBox('Cosmetic', 1, 'Player Sprite: ');
    spriteBox.setAttribute('list', 'sprites');
    spriteBox.addEventListener('change', function(event) {
        if (sprite_choices.includes(this.value)) {
            this.classList.remove('invalid');
            localStorage.setItem('sprite', this.value);
        } else {
            this.classList.add('invalid');
        }
    });
}

window.addEventListener('load', event => {
    Module.onRuntimeInitialized = () => {
        $('#version').innerText = Module.ccall('version', 'string');
        let sprite_name = Module.cwrap('sprite_name', 'string', ['number']);
        let sprites_el = $('#sprites');
        let i=0;
        while(true) {
            let name = sprite_name(i++);
            if (!name) break;
            let option = document.createElement('option');
            option.value = option.innerText = name;
            sprite_choices.push(name);
            sprites_el.append(option);
        }
    };
    setup_ui();

    $('#inputfile').addEventListener('change', function(event) {
        let file = this.files[0];
        file.arrayBuffer().then(buffer => {
            rom = new Rom(buffer);
            rom.name = file.name.basename();
            localStorage.setItem('rom_name', rom.name);
            localStorage.setItem('rom_data', rom);
        });
    })

    $('#random_seed').addEventListener('click', function(event) {
        $('#seed').value = new String(
            Math.floor(Math.random() * Number.MAX_SAFE_INTEGER));
    });
    $('#random_seed').dispatchEvent(new MouseEvent('click'));

    $('#generate').addEventListener('click', event => {
        let seed = BigInt($('#seed').value);
        let flags = $('#flags').value;
        let sprite = $('#sprite').value;
        rom.randomize(seed, flags, sprite);
        rom.save();
    })

    let rom_data = localStorage.getItem('rom_data');
    if (rom_data) {
        rom = new Rom(rom_data.split(','));
        ui.setInputFile(localStorage.getItem('rom_name');
    }

    $("#flags").value = "CDFGMPRSTWZlr";
    $("#sprite").value = "Random";
});

