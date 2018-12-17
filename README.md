[![Build Status](https://bryphe.visualstudio.com/merlin-language-server/_apis/build/status/bryphe.merlin-language-server?branchName=master)](https://bryphe.visualstudio.com/merlin-language-server/_build/latest?definitionId=16?branchName=master)

# merlin-language-server
### Simple LSP protocol wrapper for [`merlin`](https://github.com/ocaml/merlin), written in [reason](https://reasonml.github.io/)

:warning: Alpha quality

### Overview

`merlin-language-server` is a thin wrapper over the excellent [`merlin`](https://github.com/ocaml/merlin) tooling - implementing the LSP protocol on-top of [`merlin`'s protocol](TODO). 

As with `ocaml-language-server`, this is not intended as a replacement for any existing Reason / OCaml tooling, but to make it more accessible across editors.

### Why?

There are already two excellent language server implementations - [`reason-language-server`](https://github.com/jaredly/reason-language-server) and [`ocaml-language-server`](https://github.com/freebroccolo/ocaml-language-server) - why another one?

The current language-server landspace could be summarized with this table:

| __Language Server__ | Uses `merlin` | Implemented in `reason` | Supports Windows |
| --- | --- | --- | --- |
| `ocaml-language-server` | :white_check_mark: | :x: | :x: (1) |
| `reason-language-server` | :x: | :white_check_mark: | :x: (2) |
| `merlin-language-server` | :white_check_mark: | :white_check_mark: | :white_check_mark: |

> __NOTE (1):__ I believe `ocaml-language-server` may support Windows, but I was not able to get it working.

> __NOTE (2):__ `reason-language-server` mentions Windows support as a goal, but again, it did not work for me. I tried hard to get it working (submitting various PRs [#36](https://github.com/jaredly/reason-language-server/pull/36), [#130](https://github.com/jaredly/reason-language-server/pull/130), [#199](https://github.com/jaredly/reason-language-server/pull/199)), but there is still significant investment that would need to be done.

My biggest issue was that there was not a language server implementation that worked well for me on Windows. It turns out that `merlin` works great on Windows - so having a native-reason LSP implementation that uses `merlin` seems like a natural choice. 

In the future, perhaps we could integrate this more fully into `merlin` at some point, or incorporate `merlin` as a library that we call directly.

### Building

#### Prerequisites

- You will need [`esy`](https://esy.sh) to build - `npm install -g esy`
- For running tests, you will need `node`.

#### Build

- `git clone https://github.com/bryphe/merlin-language-server`
- `esy install`
- `esy build`

#### Tests
- Unit tests: `esy x TestRunner.exe`
- E2E tests: 
    - `cd test-e2e`
    - `npm install`
    - `npm test`

