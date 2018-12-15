/*
 * MerlinDiscovery
 *
 * Helper module for finding Merlin in the current environment
 */

let findMerlinInEnvironment = () => {
   let merlinPath = Rench.Environment.getEnvironmentVariable("MLS_MERLIN_PATH");
   switch (merlinPath) {
   | Some(v) => Some(Merlin.init(Merlin.Single, v));
   | None => None
   }
}

/* let findMerlinWithEsy = (rootUri) => { */
/*     let esyPath = Rench.Environment.which("esy"); */

/*     let whichOrWhere = switch(Sys.win32) { */
/*     | true => "where" */
/*     | false => "which" */
/*     }; */

/*     switch (esyPath) { */
/*     | Some(v) => { */
/*        /1* There's an esy in the environment... let's see if we can find merlin in the project *1/ */ 

/*         Rench.ChildProcess.spawnSync("esy", [|whichOrWhere, "ocamlmerlin"|]) */
/*     } */
/*     | None => None */
/*     } */
/* }; */

let discover = (_rootUri) => {
    switch (findMerlinInEnvironment()) {
    | Some(v) => v
    | None => Merlin.init(Merlin.Single, "C:\\Users\\bryph\\.esy\\3_\\i\\opam__s__merlin-opam__c__3.2.2-9e36d08c\\bin\\ocamlmerlin.exe");
    }
};
