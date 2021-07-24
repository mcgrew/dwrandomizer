
$ = document.querySelector.bind(document)
$$ = document.querySelectorAll.bind(document)

NodeList.prototype.addEventListener = function(evt_type, func) {
    this.forEach(function(node) {
        node.addEventListener(evt_type, func.bind(node));
    });
};

NodeList.prototype.hide = function() {
    this.forEach(function(node) {
        node.display = 'none';
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
            sprites_el.appendChild(option);
        }
    };

    $('#sprite').addEventListener('change', function(event) {
        if (sprite_choices.includes(this.value)) {
            this.classList.remove('invalid');
        } else {
            this.classList.add('invalid');
        }
    });

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
    }

    $("#flags").value = "CDFGMPRSTWZlr";
    $("#sprite").value = "Random";
});

