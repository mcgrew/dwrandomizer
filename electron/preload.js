// All of the Node.js APIs are available in the preload process.
// It has the same sandbox as a Chrome extension.
window.addEventListener('DOMContentLoaded', () => {
    const replaceText = (selector, text) => {
        const element = document.getElementById(selector)
        if (element) element.innerText = text
    }

    for (const type of ['chrome', 'node', 'electron']) {
        replaceText(`${type}-version`, process.versions[type])
    }

    document.body.style.backgroundColor = '#fff';

    const remote = require('electron').remote
    // add a close button
    let closeButton = createElement('img', null, {
        'position': 'absolute',
        'top': '4px',
        'right': '4px',
        'height': '16px',
        'width': '16px',
        'cursor': 'pointer',
        '-webkit-app-region': 'no-drag'
    });
    closeButton.setAttribute('src', 'close.svg');
    closeButton.addEventListener('click', event => {
        remote.getCurrentWindow().close();
    });
    document.body.prepend(closeButton);

    // add a minimize button
    let minButton = createElement('img', null, {
        'position': 'absolute',
        'top': '4px',
        'right': '30px',
        'height': '16px',
        'width': '16px',
        'cursor': 'pointer',
        '-webkit-app-region': 'no-drag'
    });
    minButton.setAttribute('src', 'minimize.svg');
    minButton.addEventListener('click', event => {
        remote.getCurrentWindow().minimize();
    });
    document.body.prepend(minButton);

    // add a "title bar"
    let titleBar = createElement('div', null, {
        'position': 'absolute',
        'top': '0px',
        'right': '0px',
        'left': '0px',
        'height': '26px',
        'background-color': '#ccc',
        '-webkit-app-region': 'drag'
    });
    document.body.prepend(titleBar);

})

function createElement(element, text, style) {
    let el = document.createElement(element);
    if (text)
        el.innerText = text
    if (style) {
        for (key in style)
            el.style.setProperty(key, style[key]);
    }
    return el;
}
