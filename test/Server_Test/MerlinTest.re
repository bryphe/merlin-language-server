open TestFramework;

module Merlin = Server.Merlin;

describe("Merlin", ({test, _}) => {
  test("fails", ({expect}) => {
      let merlinPath = String.trim(Rench.ChildProcess.spawnSync("where", [|"ocamlmerlin.exe"|]).stdout);

      let merlin = Merlin.init(merlinPath);
      let vnum = String.trim(Merlin.run(merlin, "-vnum"));
      expect.string(vnum).toEqual("v3.2.2");
  });
});
