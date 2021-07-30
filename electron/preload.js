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


    const remote = require('electron').remote
    // add a close button
    let closeButton = createElement('img', null, {
        'float': 'right',
        'margin': '4px',
        'height': '16px',
        'width': '16px',
        'cursor': 'pointer',
        '-webkit-app-region': 'no-drag'
    });
    closeButton.setAttribute('src', 'close.svg');
    closeButton.addEventListener('click', event => {
        remote.getCurrentWindow().close();
    });
    titleBar.append(closeButton);

    // add a minimize button
    let minButton = createElement('img', null, {
        'float': 'right',
        'margin': '4px',
        'height': '16px',
        'width': '16px',
        'cursor': 'pointer',
        '-webkit-app-region': 'no-drag'
    });
    minButton.setAttribute('src', 'minimize.svg');
    minButton.addEventListener('click', event => {
        remote.getCurrentWindow().minimize();
    });
    titleBar.append(minButton);

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
