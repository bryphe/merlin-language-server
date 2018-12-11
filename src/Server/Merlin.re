open Rench;

type t = {
    merlinPath: string
};

let init = (merlinPath: string) => {
    let ret: t = {
        merlinPath: merlinPath,
    };
    ret;
};

let run = (merlin: t, command: string) => {
   let proc = ChildProcess.spawnSync(merlin.merlinPath, [|"single", command |] );
   proc.stdout;
};
