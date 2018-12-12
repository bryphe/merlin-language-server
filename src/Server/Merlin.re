open Rench;

type t = {merlinPath: string};

[@deriving yojson({strict: false})]
type oneBasedLine = int;

[@deriving yojson({strict: false})]
type oneBasedCharacter = int;

module Position = {
  [@deriving yojson({strict: false})]
  type t = {
    line: oneBasedLine,
    col: oneBasedCharacter,
  };
};

[@deriving yojson({strict: false})]
type typeEnclosingResultItem = {
  tail: string,
  [@key "type"]
  enclosingType: string,
  /* start: Position.t, */
};

[@deriving yojson({strict: false})]
type typeEnclosingResult = list(typeEnclosingResultItem);

let init = (merlinPath: string) => {
  let ret: t = {merlinPath: merlinPath};
  ret;
};

let run = (~input: string, merlin: t, command: array(string)) => {
  let opts = ChildProcess.SpawnSyncOptions.create(~input, ());
  let proc =
    ChildProcess.spawnSync(
      ~opts,
      merlin.merlinPath,
      Array.append([|"single"|], command),
    );
  proc.stdout;
};
