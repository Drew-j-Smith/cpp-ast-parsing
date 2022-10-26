var Module = {
    print(text) {
        let output = document.getElementById("output");
        let p = document.createElement("p")
        p.textContent = text;
        output.appendChild(p);
    },
    printErr(text) { console.warn(text); },
    onRuntimeInitialized() {
        let wasm_parse = this.cwrap("wasm_parse", null, ["string"]);
        let input = document.getElementById("input");
        let button = document.getElementById("button");
        let output = document.getElementById("output");
        button.onclick = () => {
            let p = document.createElement("code");
            p.textContent = input.value;
            output.appendChild(p);
            wasm_parse(input.value);
            input.value = "";
        }
    }
};