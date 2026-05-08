# Digital Logic Simulator

A desktop app that takes any boolean expression and instantly shows you the circuit diagram, truth table, K-Map, and the simplified SOP expression — all in real time.

Built with C++17 and SFML. No online tools, no browser, just a native app that runs on your machine.

![screenshot](assets/screenshots/demo.png)

---

## What it does

You type something like `(A&B) ^ (B&C ^ A&C)` and hit Run. It gives you:

- **Circuit Diagram** — draws the actual logic gates (AND, OR, NOT, XOR) wired together
- **Truth Table** — generates all input combinations and their outputs, highlights the row you click
- **Karnaugh Map** — 2, 3, or 4 variable K-Map drawn automatically
- **Simplified Expression** — runs Quine-McCluskey on the minterms and gives you the minimal SOP form
- **Gate Stats** — gate count, circuit depth, input count, minterm ratio shown at a glance
- **Screenshot** — saves the circuit as an image file
- **Export** — exports the truth table and expression to a `.txt` file
- **Dark/Light mode** toggle

---

## Tech stack

- **C++17**
- **SFML 2.6** — for the window, rendering, and fonts
- **Quine-McCluskey algorithm** — implemented from scratch for SOP minimization
- **Recursive descent parser** — tokenizes and parses boolean expressions into an AST
- **Custom circuit layout engine** — places gates and wires automatically based on tree depth and leaves

---

## Supported operators

| Symbol | Meaning |
|--------|---------|
| `&`    | AND     |
| `\|`   | OR      |
| `^`    | XOR     |
| `!`    | NOT     |
| `( )`  | Grouping |

Variables can be any single uppercase letter like `A`, `B`, `C`, `D`.

---

## Build & Run

**Requirements**
- g++ with C++17 support
- SFML 2.6 dev libraries

**Install SFML (Ubuntu/Kubuntu)**
```bash
sudo apt install libsfml-dev
```

**Compile**
```bash
g++ -std=c++17 main.cpp GUI.cpp Parser.cpp Tokenizer.cpp TruthTable.cpp CircuitDrawer.cpp KMapDrawer.cpp \
$(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system) \
-o logic_sim
```

**Run**
```bash
./logic_sim
```

Make sure the `assets/fonts/` folder is in the same directory as the binary.

---

## Project structure

```
├── main.cpp           — entry point
├── GUI.cpp/h          — main window, layout, event handling
├── Parser.cpp/h       — recursive descent boolean expression parser
├── Tokenizer.cpp/h    — splits raw expression string into tokens
├── Node.h             — AST node types (AndGate, OrGate, NotGate, XorGate, InputNode)
├── TruthTable.cpp/h   — truth table generation + Quine-McCluskey simplifier
├── CircuitDrawer.cpp/h — automatic gate placement and wire routing
├── KMapDrawer.cpp/h   — Karnaugh map renderer (2–4 variables)
└── assets/fonts/      — font files
```

---

## Example expressions to try

```
A & B
A | !B
(A & B) | (B & C)
(A&B) ^ (B&C ^ A&C)
(A&B^B&C) ^C^A
```

---

## About

Made by **Turjo** — CSE student, Digital Logic Design course project.  
Runs fully offline. No dependencies beyond SFML.

Feel free to fork it, break it, or improve it.

---

## License

MIT — do whatever you want with it.
