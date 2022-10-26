const input = document.getElementById("input");
const button = document.getElementById("button");
const output = document.getElementById("output");
var Module = {
    print(text) {
        let p = document.createElement("p")
        p.textContent = text;
        output.appendChild(p);
    },
    printErr(text) { console.warn(text); },
    onRuntimeInitialized() {
        let wasm_parse = this.cwrap("wasm_parse", null, ["string"]);
        button.onclick = () => {
            let p = document.createElement("code");
            p.textContent = input.value;
            output.appendChild(p);
            wasm_parse(input.value);
            input.value = "";
        }
    }
};