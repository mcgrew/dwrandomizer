

/**
 * A class for creating a modal window
 */
class Modal {
    constructor(width, height, title, okText, cancelText, okFunc) {
        if (Modal._instance)
            throw 'Only one instance of this class can exist at once';
        Modal._instance = this;
        this.main = this.create('div', null, {
            'position': 'absolute',
            'top': '0',
            'bottom': '0',
            'left': '0',
            'right': '0',
        });
        this.overlay = this.create('div', null, {
            'position': 'absolute',
            'top': '0',
            'bottom': '0',
            'left': '0',
            'right': '0',
            'background-color': 'black',
            'opacity': '0.5'
        });
        this.overlay.click(() => { this.close(); });
        document.body.append(this.main);
        this.main.append(this.overlay);
        let dialogOffset = (this.overlay.offsetHeight - height) / 2;
        this.dialog = this.create('div', null, {
            'padding': '0.2em',
            'outline': '1px solid black',
            'font-weight': 'bold',
            'text-align': 'center',
            'margin': 'auto',
            'position': 'relative',
            'margin-top': dialogOffset + 'px',
            'width': width + 'px',
            'height': height + 'px',
            'z-index': '3'
        });
        this.dialog.classList.add('modal');
        this.main.prepend(this.dialog);
        if (title) {
            let heading = this.create('h3', title);
            this.dialog.append(heading);
        }
        let canceler = this.create('div', null, {
            'position': 'absolute',
            'bottom': '0.5em',
            'right': '1em',
        });
        this.dialog.append(canceler);
        if (okText) {
            let okButton = this.create('button', okText)
            okButton.click(okFunc);
            canceler.append(okButton);
        }
        if (cancelText) {
            let cancelButton = this.create('button', cancelText)
            cancelButton.click(evt => {
                this.close();
            });
            canceler.append(cancelButton);
        }
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
    close() {
        this.main.remove();
        Modal._instance = null;
    }
}

/**
 * Creates a modal for loading & storing flags
 */
class FlagsModal extends Modal {
    constructor() {
        super(400, 300, 'Presets', null, 'Cancel');
        let builtinPresets = {
            'Standard': 'IVIAAVCAKACAAAAAAAAAAEAA',
            'Fast Standard': 'IVIAAVCAKACFAAAAAAAAAIAA',
            'Easy Standard': 'IVIAAVCFKACACQAAAAAAAZAA',
            'Tiebreaker': 'IVKQAVCEKEAFKRIAAAAAAYAA',
        }
        this.dialog.append(this.create('div', 'Save', {
            'font-weight': 'normal',
            'margin-bottom': '0.5em',
        }));
        let saver = this.create('div');
        let nameBox = this.create('input');
        nameBox.placeholder = 'Preset Name';
        nameBox.setAttribute('maxlength', '50');
        saver.append(nameBox);
        let saveButton = this.create('button', 'Save current');
        saver.append(saveButton)
        this.dialog.append(saver);
        this.dialog.append(saver)
        let flagSelect = this.create('select');
        for (let [key,value] of Object.entries(builtinPresets)) {
            let opt = this.create('option', key)
            opt.value = value
            flagSelect.append(opt);
        }
        this.dialog.append(this.create('div', 'Load/Delete', {
            'font-weight': 'normal',
            'margin-top': '2em',
            'margin-bottom': '0.5em',
        }));
        let loader = this.create('div');
        let storedPresets = localStorage.getItem('storedPresets');
        if (storedPresets) {
            storedPresets = JSON.parse(storedPresets);
            for (let [key,value] of Object.entries(storedPresets)) {
                let opt = this.create('option', key)
                opt.value = value
                flagSelect.append(opt);
            }
        } else {
            storedPresets = {};
        }
        loader.append(flagSelect)
        let loadButton = this.create('button', 'Load preset', {
            'margin-left': '1em',
        });
        let deleteButton = this.create('button', 'Delete Preset', {
            'margin-left': '1em',
        });
        loader.append(this.create('br'));
        loader.append(this.create('br'));
        loader.append(loadButton)
        loader.append(deleteButton)
        this.dialog.append(loader)
        // init events
        saveButton.click(evt => {
            if (nameBox.value) {
                storedPresets[nameBox.value] = Interface.getFlags()
                localStorage.setItem('storedPresets',
                    JSON.stringify(storedPresets));
                this.close()
            }
        });
        loadButton.click(evt => {
            Interface.setFlags(flagSelect.value)
            this.close();
        });
        deleteButton.click(evt => {
            let option = flagSelect.selectedOptions[0]
            delete storedPresets[option.innerText];
            localStorage.setItem('storedPresets',
                JSON.stringify(storedPresets));
            option.remove();
        });
        deleteButton.disabled = 'true';
        flagSelect.change(function() {
            if (this.selectedIndex < Object.keys(builtinPresets).length) {
                deleteButton.disabled = 'true';
            } else {
                deleteButton.disabled = undefined;
            }
        });
    }
}
