
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
    if (window.matchMedia("(prefers-color-scheme: dark)").matches) {
        document.body.style.backgroundColor = '#333';
    }
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
