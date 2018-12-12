open TestFramework;

module Merlin = Server.Merlin;

let startMerlin = () => {
      let merlinPath = String.trim(Rench.ChildProcess.spawnSync("where", [|"ocamlmerlin.exe"|]).stdout);
      Merlin.init(merlinPath);
};

describe("Merlin", ({test, _}) => {
  test("get version", ({expect}) => {
      let merlin = startMerlin();
      let vnum = String.trim(Merlin.run(~input="", merlin, [|"-vnum"|]));
      expect.string(vnum).toEqual("v3.2.2");
  });
  test("simple type-enclosing case from docs", ({expect}) => {
      let merlin = startMerlin();
      let output = String.trim(Merlin.run(~input="let a = 1\r\n", merlin, [|"type-enclosing", "-position \"1:5\"", "-filename test.ml"|]));
      let json = Yojson.Safe.from_string(output);
      let res = Yojson.Safe.Util.member("value", json) |> Merlin.typeEnclosingResult_of_yojson |> Protocol.Utility.getResultOrThrow;
      expect.string(List.hd(res).enclosingType).toEqual("int");
  });
});
