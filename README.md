# Clara

**An interactive C++ handwriting-recognition experiment powered by AlphaCore.**  
**2023 Congressional App Challenge winner**

Clara is a desktop application that recognizes handwritten **digits (0–9)** drawn on a canvas and provides spoken feedback. The project was motivated by the possibility of making handwriting-related learning tools more accessible, including for students who experience difficulties with writing and recognition.

Rather than relying on a high-level inference API, Clara uses **[AlphaCore](https://github.com/TheNextGreatestThing/AlphaCore)**, a custom C++ neural-network library, together with OpenGL/GLUT for drawing and macOS text-to-speech for audio output.

> **Project status:** This is an experimental application, not a validated educational or assistive technology. The published source demonstrates digit recognition and spoken feedback; it does **not** currently demonstrate automatic retraining from user corrections or measure outcomes for learners with dyslexia or dysgraphia.

## Features

- **Interactive handwriting canvas:** Captures mouse-drawn strokes through an OpenGL/GLUT desktop interface.
- **28 × 28 input representation:** Converts the drawing surface into a 784-element input vector and scales pixel intensity values for inference.
- **Two neural network configurations:** Loads a sigmoid-based model and a ReLU-based model, compares their output scores, and selects a prediction.
- **Confidence-gated responses:** Uses a margin between the top two output scores to decide whether to speak a recognized digit.
- **Audio feedback:** Announces predictions using the macOS `say` command and the `Ava` voice.
- **Reusable ML infrastructure:** Uses AlphaCore for feedforward computation, activation functions, and model loading rather than implementing the neural-network framework inside the app.

## How it works

```text
Mouse-drawn handwriting
        |
        v
784 x 784 drawing canvas
        |
        v
28 x 28 input representation
        |
        v
Normalize to 784 values in [0, 1]
        |
        +------------------------+
        |                        |
        v                        v
Sigmoid-based network     ReLU-based network
        |                        |
        +-----------+------------+
                    |
                    v
          Compare output scores
                    |
                    v
       Check confidence margin
                    |
                    v
     Speak recognized digit (0-9)
```

### Model architecture

Both networks use the same layer dimensions:

```text
784 -> 1012 -> 816 -> 508 -> 240 -> 10
```

The models differ in their hidden-layer activation functions: one uses sigmoid activations; the other uses ReLU. Both use softmax at the output layer. In the application, the model parameters are **loaded from previously saved files** rather than trained from scratch at startup.

The application takes the highest-scoring output from each network and chooses the network with the higher maximum output score. It speaks a digit when the configured confidence-margin condition is met.

## Technologies

| Component | Technology |
| --- | --- |
| Application | C++ |
| Neural network | AlphaCore (C++), Eigen backend |
| Interface and rendering | OpenGL, GLUT |
| Audio output | macOS `say` command |
| Platform-specific executable paths | macOS Mach-O APIs |
| Optional dataset utilities | MNIST-format image and label readers in `Tools.h` |

## Repository structure

```text
Clara/
├── README.md          # Project overview and setup
├── main.cpp           # Application, inference, UI callbacks
├── Tools.h            # Drawing, data, and utility functions
└── AlphaCore.h        # Renamed legacy AlphaCore header used by this project
```

**Model weights are not included in these source files.** To run a functional inference demo, provide compatible trained-model directories (or use an existing release bundle that includes them).

## Building on macOS

Clara's current source is **macOS-oriented** and uses Apple's GLUT/OpenGL framework and `say` speech synthesis. Other operating systems would require modifications.

**Requirements:**

- A C++17-capable compiler, such as Apple Clang
- Eigen headers available on the compiler include path
- macOS development headers/frameworks for OpenGL and GLUT (availability varies by macOS/Xcode SDK)
- Trained AlphaCore model files for both networks

With those dependencies installed, a representative build command from the project directory is:

```bash
clang++ -std=c++17 main.cpp -o Clara \
  -I/path/to/eigen3 \
  -framework OpenGL -framework GLUT
```

Replace `/path/to/eigen3` with the directory containing the `Eigen/` headers. **This command has not been validated against a current macOS SDK**; legacy GLUT headers may require adjustments.

### Model files

The application looks for the following directories, containing parameters serialized by AlphaCore:

```text
Clara/
├── Clara                 # Compiled executable
├── trainedSigmoid/       # Saved weights and biases for the sigmoid model
└── trainedLarge/         # Saved weights and biases for the ReLU model
```

Each model directory is expected to contain layer-wise weight and bias files named `w0.csv`, `b0.csv`, etc. The exact dimensions must match the architecture above. If a `UserData` directory exists in the **current working directory**, the app instead attempts to load that directory for both model instances.

The original code derives the model location from the executable path by finding the last `C` in that path. Keeping the executable named `Clara` and the model directories beside it matches the original layout, but this path-resolution approach is fragile. It should be replaced with normal filesystem path handling for portability.

Run the executable from the project directory once the required weights are present:

```bash
./Clara
```

## Source code notes

- `main.cpp` configures the two networks, processes pointer events, performs inference, and triggers audio feedback.
- `Tools.h` includes OpenGL drawing helpers, numerical utilities, and functions for reading MNIST-format data.
- `AlphaCore.h` is the older library snapshot used by Clara, updated from its original `DeepLearning` naming to the `AlphaCore` namespace and `AC_*` macros. It is provided here to preserve this project's API. It may differ slightly from the current standalone AlphaCore repository.

## Limitations and future improvements

This repository is a record of an independent research/application project, rather than a production-ready accessibility product.

- **No published evaluation metrics:** Accuracy, latency, robustness, and user-study results are not established by the provided source.
- **No live personalization shown:** The app can load saved model parameters, but the supplied application source does not implement a correction-driven retraining loop.
- **Platform dependency:** The UI, executable-path handling, and speech synthesis assume macOS.
- **Reproducibility:** Model weights and a supported build environment are required for running inference.
- **Input handling:** Drawing-coordinate bounds, model-score calibration, and model-file handling would benefit from additional validation and testing.

A short screen recording, representative handwritten-digit examples, and documented model evaluation would make the repository easier to assess.

## Recognition

Clara received a **2023 Congressional App Challenge** award. The project was created as an independent exploration of neural-network-based handwriting recognition and accessible feedback.

## Related project

- **[AlphaCore](https://github.com/TheNextGreatestThing/AlphaCore)** — the underlying C++ neural-network library also used in Binocular.

