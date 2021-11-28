
require('modal.js');

/**
 * Adds an eventlisterner to the built-in NodeList type
 */
NodeList.prototype.addEventListener = function(evt_type, func) {
    this.forEach(function(node) {
        node.addEventListener(evt_type, func.bind(node));
    });
};

/**
 * Adds a hide() method to the built-in NodeList type which hides all
 * elements in the list (sets display to none).
 */
NodeList.prototype.hide = function() {
    this.forEach(function(node) {
        node.hide()
    });
}

/**
 * Adds a hide() method to the HTMLElement type (sets display to 'none')
 */
HTMLElement.prototype.hide = function() {
    this.style.display = 'none';
}

/**
 * Adds a show() method to the HTMLElement type (clears display setting)
 */
HTMLElement.prototype.show = function() {
    this.style.display = '';
}

/**
 * Adds a click() method to the HTMLElement type. When called without
 * arguments, this emulates a click on the element. When called with a
 * function, this adds a click event listener
 *
 * @param func The function to execute when the element is clicked.
 */
HTMLElement.prototype.click = function(func) {
    if (func)
        this.addEventListener('click', func);
    else
        this.dispatchEvent(new MouseEvent('click'));
}

/**
 * Adds a change() method to the HTMLElement type.
 * This adds a change event listener to the element.
 *
 * @param func The function to execute when the element is changed. Intended
 * for input fields.
 */
HTMLElement.prototype.change = function(func) {
    this.addEventListener('change', func);
}

/**
 * Adds a getValue() method to the HTMLSelectElement type. Converts the
 * value to a number and returns it.
 */
HTMLSelectElement.prototype.getValue = function () {
    return Number(this.value);
}

/**
 * Adds a setValue() method to the HTMLSelectElement type. Sets the selected
 * option to the provided value.
 *
 * @param value The index of the option to select.
 */
HTMLSelectElement.prototype.setValue = function (value) {
    let maxvalue = 0;
    for (let i=0; i < this.children.length; i++) {
        if (maxvalue < Number(this.children[i].value))
            maxvalue = Number(this.children[i].value)
    }
    // mask off the bits we don't need
    let mask = Math.pow(2, Math.floor(Math.log2(maxvalue)) + 1) - 1;
    value &= mask;
    for (let i=0; i < this.children.length; i++) {
        if (value == Number(this.children[i].value)) {
            this.selectedIndex = i;
            break;
        }
    }
}

/**
 * Adds a getValue() method to the HTMLInputElement type. Converts the
 * value to a number and returns it.
 */
HTMLInputElement.prototype.getValue = function() {
    if (this.type == 'checkbox'){
        if (this.indeterminate)
            return 2;
        else if (this.checked)
            return 1;
        else
            return 0;
    } else {
        return Number(this.value);
    }
}

/**
 * Adds a setValue() method to the HTMLInputElement type. Only works on
 * checkboxes. Sets the value to off (0), on (1), or indeterminate (2).
 * to the provided value.
 *
 * @param value The value to set the checkbox to.
 */
HTMLInputElement.prototype.setValue = function (value) {
    if (this.type == 'checkbox') {
        if (this.dataset.tristate) {
            value &= 3;
        } else {
            value &= 1;
        }
        this.checked = this.indeterminate = false;
        if (value & 2) {
            this.indeterminate = true;
            this.dataset.indeterminate = true;
        } else if (value & 1) {
            this.checked = true;
        }
    }
}

/**
 * Converts a normal checkbox to a tri-state checkbox
 */
HTMLInputElement.prototype.triState = function() {
    if (this.type == 'checkbox') {
        this.dataset.tristate = true
        this.change(function(event) {
            if (!this.checked) {
                this.indeterminate = true;
                this.dataset.indeterminate = true;
            } else if (this.dataset.indeterminate) {
                delete this.dataset.indeterminate;
                this.checked = false;
            }
        });
    }
}

/**
 * Adds a basename() function to the String type. This will remove the path
 * from a file path leaving only the file name
 */
String.prototype.basename = function() {
    let index = this.lastIndexOf('/');
    if (index < 0)
        index = this.lastIndexOf('\\');
    return this.substring(index + 1);
}

/**
 * Adds an isNumeric() function to the String type. Returns true if the String
 * can be interpreted as a number
 */
String.prototype.isNumeric = function() {
    return !!/^[0-9]*(.[0-9]*)?$/.exec(this)
}

window.addEventListener('hashchange', event => {
    if (!ignoreHashChange && Interface.instance)
        Interface.instance.readHash();
});

/**
 * The main user interface class
 */
class Interface {
    constructor(flagSize) {
        if (Interface._instance)
            throw 'Only one instance of this class can exist at once';
        Interface._instance = this;
        this.flagSize = flagSize;
        this.flagBytes = new Uint8Array(flagSize);
        this.inputs = [];
        this.appContainer = $('#application');
        this.title = this.create('img', null, {
            'display': 'block',
            'margin': 'auto',
        });
        this.title.src = 'logo.png';
        this.title.alt = 'Dragon Warrior Randomizer';
        this.appContainer.append(this.title);
        this.subHeader = this.create('h3', null, {
            'margin-top': '0.2em'
        });
        this.appContainer.append(this.subHeader);

        this.inputFileDiv = this.create('div', 'Rom File: ', {
            'padding': '0.2em'
        });
        this.inputFile = this.create('input', null, {
            'position': 'absolute',
            'opacity': '0',
            'cursor': 'pointer'
        })
        this.filenameSpan = this.create('span',
            localStorage.getItem('rom_name') || 'No file selected')
        this.inputFile.type = 'file';
        this.inputFileDiv.append(this.inputFile)
        this.inputFileDiv.append(this.filenameSpan);
        this.appContainer.append(this.inputFileDiv);

         let flagsSeed = this.create('div', null, {
            'display': 'grid',
            'grid-template-columns': '50% 50%',
            'padding': '0.2em'
        });
        let flagsDiv = this.create('div', 'Flags: ');
        this.flagsEl = this.create('input', null, {
            'width':  '240px'
        });
        this.flagsEl.id = 'flags';
        this.flagsEl.value = localStorage.flags
        this.updateFlagBytes();
        flagsDiv.append(this.flagsEl);
        let presetsButton = this.create('button', 'Presets');
        flagsDiv.append(presetsButton);
        presetsButton.click(evt => { new FlagsModal() });
        flagsSeed.append(flagsDiv);

        let seedDiv = this.create('div', 'Seed: ')
        this.seedEl = this.create('input');
        this.seedEl.type = 'number'
        this.seedEl.value = new String(
            Math.floor(Math.random() * Number.MAX_SAFE_INTEGER));
        seedDiv.append(this.seedEl);
        this.seedButton = this.create('button', 'Random Seed');
        seedDiv.append(this.seedButton);
        flagsSeed.append(seedDiv);
        this.appContainer.append(flagsSeed)

        this.tabBar = this.create('tabbar', null, {
            'padding-top': '0.5em'
        });
        this.appContainer.append(this.tabBar);
        this.tabContainer = this.create('tabcontainer');
        this.appContainer.append(this.tabContainer);
        this.tabs = {};
        let goContainer = this.create('div', null, {
            'text-align': 'right',
            'padding-top': '0.5em'
        });
        this.goButton = this.create('button', 'Randomize!');
        goContainer.append(this.goButton);
        this.appContainer.append(goContainer);
        this.checksumHolder = this.create('input');
        this.checksumHolder.readOnly = true;
        this.checksumButton = this.create('button', 'Copy');
        this.checksumContainer = this.create('div', 'Checksum: ');
        this.checksumContainer.append(this.checksumHolder);
        this.checksumContainer.append(this.checksumButton);
        this.checksumContainer.hide();
        this.appContainer.append(this.checksumContainer);
        this.initEvents();
        this.readHash();
        Interface.instance = this;
    }

    /**
     * Creates a new HTML element
     *
     * @param element The element type (e.g. div, span, etc)
     * @param text Text to be inserted into the element. Pass null/undefined
     *      to leave the element empty
     * @param style An object containing any additional style attributes to be
     *      added to the object
     */
    create(element, text, style) {
        let el = document.createElement(element);
        if (text)
            el.innerText = text
        if (style) {
            for (let key in style)
                el.style.setProperty(key, style[key]);
        }
        return el;
    }

    /**
     * Sets up user interface events
     */
    initEvents() {
        this.inputFile.change(function(iface, event) {
            let file = this.files[0];
            file.arrayBuffer().then(buffer => {
                rom = new Rom(buffer);
                rom.name = file.name.basename();
                iface.setInputFile(rom.name);
                localStorage.setItem('rom_name', rom.name);
                localStorage.setItem('rom_data', rom);
            });
        }.bind(this.inputFile, this));

        this.seedButton.click(event => {
            this.setSeed(new String(
                Math.floor(Math.random() * Number.MAX_SAFE_INTEGER)));
        });

        this.flagsEl.change(event => {
            // remove invalid characters and pad out the value
            let charsNeeded = Math.ceil(this.flagSize / 5 * 8);
            let val = this.flagsEl.value.toUpperCase();
            val = val.replace(/[^ABCDEFGHIJKLMNOPQRSTUVWXYZ234567]/g, '')
            val = val.substring(0,charsNeeded);
            while (val.length < charsNeeded) {
                val += 'A';
            }
            this.flagsEl.value = val;
            this.updateFlagBytes();
            this.updateInputs();
            this.setHash();
        });
        this.flagsEl.addEventListener('keydown', function(evt) {
            // Prevent the user from typing invalid characters
            if (evt.key.length > 1)
                return true;
            if (evt.ctrlKey || evt.altKey || evt.metaKey)
                return true;
            if ('ABCDEFGHIJKLMNOPQRSTUVWXYZ234567'.indexOf(
                    evt.key.toUpperCase()) < 0) {
                evt.preventDefault();
                return false;
            }
            return true;
        });

        this.seedEl.change(event => {
            this.setHash();
        });

        this.goButton.click(event => {
            let seed = BigInt(this.seedEl.value);
            let flags = this.flagsEl.value;
            let sprite = localStorage.sprite || 'Random';
            let checksum = rom.randomize(seed, flags, sprite);
            this.showChecksum(true, checksum.toString(16));
            this.checksumHolder.value = checksum.toString(16);
            rom.save();
        });
        this.checksumButton.click(event => {
            this.checksumHolder.select();
            document.execCommand('copy');
            this.checksumHolder.setSelectionRange(0, 0);
        });
    }

    /**
     * Reads and parses the URL hash for flags & seed number
     */
    readHash() {
        let flags = '';
        let seed = '';
        if (location.hash) {
            let query = location.hash.substring(1);
            query.split('&').forEach(item => {
                if (item.includes('=')) {
                    let params = item.split('=');
                    if (params[0] == 'flags')
                        this.setFlags(params[1]);
                    if (params[0] == 'seed')
                        this.setSeed(params[1]);
                }
            })
        }
    }

    /**
     * Updates the URL hash with the flags & seed number
     */
    setHash() {
        window.ignoreHashChange = true;
        let decoded = base32.decode(this.flagsEl.value)
        let retain = base32.decode(localStorage.retainFlags);
        for (let i=0; i < decoded.length; i++) {
            decoded[i] &= ~retain[i];
        }
        let flags = base32.encode(decoded);
        location.hash = ('flags=' + flags + '&seed=' + this.seedEl.value);
        window.ignoreHashChange = false;
    }

    /**
     * Sets the seed to the given number
     *
     * @param seed The new seed number
     */
    setSeed(seed) {
        this.seedEl.value = seed;
        this.setHash();
    }

    /**
     * Sets the flags to the provided value (base32 encoded)
     *
     * @param the new flags value
     */
    setFlags(flags) {
        this.flagsEl.value = flags;
        localStorage.setItem('flags', flags);
        this.updateFlagBytes();
        this.updateInputs();
        this.updateSummary()
        this.setHash();
    }

    /**
     * Static version of setFlags()
     */
    static setFlags(flags) {
        return Interface._instance.setFlags(flags);
    }

    /**
     * Gets the current flag string
     */
    static getFlags() {
        return Interface._instance.flagsEl.value;
    }

    /**
     * Updates the URL hash and summary tabs. Returns the
     * base32 encoded flags.
     *
     * @return The base32 encoded flags
     */
    updateFlags() {
        this.flagBytes.fill(0);
        let retain = new Uint8Array(this.flagSize)
        for (let i=0; i < this.inputs.length; i++) {
            let input = this.inputs[i];
            let bytepos = Number(input.dataset.bytepos)
            let shift =  Number(input.dataset.shift)
            this.flagBytes[bytepos] |= input.getValue() << shift;
            if (input.dataset.retain)
                retain[bytepos] |= input.getValue() << shift;
        }
        let encoded = base32.encode(this.flagBytes);
        localStorage.setItem('flags', encoded);
        localStorage.setItem('retainFlags', base32.encode(retain));
        this.flagsEl.value = encoded;
        this.updateSummary()
        this.setHash();
        return encoded;
    }

    /**
     * Updates the flag bytes based upon the value in the flags input field
     */
    updateFlagBytes() {
        this.flagBytes = base32.decode(this.flagsEl.value);
        if (localStorage.retainFlags) {
            let decoded = base32.decode(localStorage.retainFlags);
            for (let i=0; i < decoded.length; i++) {
                this.flagBytes[i] |= decoded[i];
            }
        }
        let flags = base32.encode(this.flagBytes);
        this.flagsEl.value = flags;
        return this.flagBytes;
    }

    /**
     * Updates all input fields based on the current flags
     */
    updateInputs() {
        for (let i=0; i < this.inputs.length; i++) {
            let input = this.inputs[i];
            let bytepos = Number(input.dataset.bytepos)
            let shift =  Number(input.dataset.shift)
            let value = this.flagBytes[bytepos] >> shift;
            input.setValue(value);
        }
    }

    /**
     * Updates the summary tab based on the current settings
     */
    updateSummary() {
        if (!this.summary)
            return;
        this.summary.innerHTML = 'Flags: ' + this.flagsEl.value +  "\n";
        this.summary.innerHTML += 'Seed: ' + this.seedEl.value +  "\n";
        let state;
        let name;
        for (let i=0; i < this.inputs.length; i++) {
            let input = this.inputs[i];
            if (input.tagName == 'INPUT') {
                let input = this.inputs[i];
                if (input.indeterminate) {
                    state = 'Maybe';
                } else if (input.checked) {
                    state = 'Yes';
                } else {
                    state = 'No';
                }
            }
            else if (input.tagName == 'SELECT') {
                state = input.selectedOptions[0].innerText;
            }
            this.summary.innerHTML += input.dataset.label + ': ' + state + "\n";
        }
        let spriteBox = $('#sprite-box')
        if (spriteBox)
            this.summary.innerHTML += 'Player Sprite: ' + spriteBox.getValue();
    }

    /**
     * Adds a summary tab to the UI
     */
    addSummaryTab(name) {
        let tab = this.create('tab');
        tab.innerText = name;
        tab.id = name + '-tab';
        tab.click(function(event) {
            ui.setActiveTab(this.innerText);
        });
        this.tabBar.append(tab);
        let content = this.create('tabcontent', null, {
            'padding': '0',
            'grid-template-columns': '100%',
            'grid-template-rows': 'calc(250px + 6em)'
        });
        this.summary = this.create('pre', null, {
            'margin': '0',
            'padding': '1em',
            'font-size': '14px',
            'overflow-y': 'scroll',
            'user-select': 'text'
        });
        this.tabContainer.append(content);
        content.append(this.summary);
        this.tabs[name] = {
            'tab': tab,
            'content': content
        }
    }

    /**
     * Adds a new tab to the user interface
     *
     * @param name The name of the tab
     */
    addTab(name) {
        let tab = this.create('tab');
        tab.innerText = name;
        tab.id = name + '-tab';
        tab.click(function(event) {
            ui.setActiveTab(this.innerText);
        });
        this.tabBar.append(tab);
        let content = this.create('tabcontent');
        content.id = name + '-flags'
        for (let i=0; i < 12; i++) {
            content.append(this.create('flagcontainer'))
        }
        this.tabContainer.append(content);
        this.tabs[name] = {
            'tab': tab,
            'content': content
        }
    }

    /**
     * Sets the checksum value feild and displays it
     */
    showChecksum(show, value) {
        if (show === false) {
            this.checksumContainer.hide()
        } else {
            this.checksumContainer.show()
            if (value) this.checksumHolder.value = value;
        }
    }

    /**
     * Sets the version displayed in the title bar and header
     *
     * @param version The version number
     */
    setVersion(version) {
        this.subHeader.innerText = 'Version ' + version;
        document.title = 'DWRandomizer ' + version;
    }

    /**
     * Sets the name of the input file to be displayed
     *
     * @param The file name
     */
    setInputFile(name) {
        this.filenameSpan.innerText = name;
    }

    /**
     * Sets which tab is active
     *
     * @name The name of the tab to activate.
     */
    setActiveTab(name) {
        $$('tab').forEach(function(tab) { tab.classList.remove('active') });
        $$('tabcontent').hide();
        if (this.tabs[name]) {
            this.tabs[name].tab.classList.add('active');
            this.tabs[name].content.show();
        }

    }

    /**
     * Adds a text box to the UI inside a tab
     *
     * @param tab The tab to place the text box in
     * @param position A number indicating where to place the box within the
     *      tab
     * @param title The title of the text box.
     *
     * @return The newly created HTML input element.
     */
    addTextBox(tab, position, title) {
        let input = this.create('input')
        input.name = tab + position;
        let label = this.create('label', title + ': ')
        label.for = tab + position;
        let container = this.tabs[tab].content.children[position];
        container.innerHTML = '';
        container.append(label);
        container.append(input);
        return input;
    }

    /**
     * Adds a dropdown menu to the UI inside a tab
     *
     * @param tab The tab to place the dropdown in
     * @param position A number indicating where to place the dropdown within
     *      the tab
     * @param bytepos which flag byte should be modified by this dropdown
     * @param shift The number of bits the value should be shifted by within
     *      the given byte.
     * @param title The title of the Dropdown.
     * @param values The list of values for the dropdown
     * @param skipFlags Whether flags should be updated by this dropdown.
     *      True causes changes to this dropdown to be ignored.
     *
     *  @return The newly created HTML SELECT element.
     */
    addDropDown(tab, position, bytepos, shift, title, values, skipFlags) {
        let select = this.create('select');
        select.dataset.bytepos = bytepos;
        select.dataset.shift = shift;
        if (values) {
            for (let v in values) {
                let option = this.create('option', v);
                option.value = values[v];
                select.append(option);
            }
        }
        select.change(this.updateFlags.bind(this));
        let container = this.tabs[tab].content.children[position];
        container.style.marginRight = '30%';
        container.style.textAlign = 'right';
        container.innerText = (select.dataset.label = title) + ': ';
        container.append(select);
        if (!skipFlags)
            this.inputs.push(select);
        this.updateInputs();
        return select;
    }

    /**
     * Adds a checkbox to the UI inside a tab
     *
     * @param tab The tab to place the checkbox in
     * @param position A number indicating where to place the checkbox within
     *      the tab
     * @param bytepos which flag byte should be modified by this checkbox
     * @param shift The number of bits the value should be shifted by within
     *      the given byte.
     * @param title The title of the Dropdown.
     * @param description An optional description to add below the checkbox.
     * @param values The list of values for the checkbox
     * @param skipFlags Whether flags should be updated by this checkbox.
     *      True causes changes to this checkbox to be ignored.
     *
     *  @return The newly created HTML INPUT element.
     */
    addOption(tab, position, bytepos, shift, title, description, retain,
            skipFlags) {
        let input = this.create('input')
        input.type = 'checkbox';
        input.name = tab + position;
        input.dataset.bytepos = bytepos;
        input.dataset.shift = shift;
        input.dataset.label = title;
        let label = this.create('label', title)
        label.for = tab + position;
        let container = this.tabs[tab].content.children[position];
        container.innerHTML = '';
        label.prepend(input);
        container.append(label);
        if (description) {
            let descriptionEl = this.create('div', description, {
                'font-size': '0.7em',
                'color': '#666',
                'margin-left': '2em',
            });
            container.append(descriptionEl);
        }
        if (retain)
            input.dataset.retain = 'true';
        if (!skipFlags) {
            input.change(this.updateFlags.bind(this));
            this.inputs.push(input);
            this.updateInputs();
        }
        return input;
    }

    /**
     * Adds a checkbox to the UI inside a tab
     *
     * @param tab The tab to place the checkbox in
     * @param position A number indicating where to place the checkbox within
     *      the tab
     * @param bytepos which flag byte should be modified by this checkbox
     * @param shift The number of bits the value should be shifted by within
     *      the given byte.
     * @param title The title of the Dropdown.
     * @param description An optional description to add below the checkbox.
     * @param values The list of values for the checkbox
     *
     *  @return The newly created HTML INPUT element.
     */
    addTriOption(tab, position, bytepos, shift , title, description) {
        let input = this.addOption(tab, position, bytepos, shift, title,
            description);
        input.triState();
        input.change(this.updateFlags.bind(this));
        this.updateInputs();
        return input;
    }
}

