open TestFramework;

module Merlin = Server.Merlin;

let startMerlin = () => {
  let merlinPath = Rench.Environment.which("ocamlmerlin");
  switch (merlinPath) {
  | Some(v) => Merlin.init(v)
  | None => Merlin.init("ocamlmerlin");
  };
};

/* Sample case from:
 * https://github.com/ocaml/merlin/blob/master/doc/dev/PROTOCOL.md
 */

let testFile = {|
let x = 5
let y = 3.0 *. x
|};

describe("Merlin", ({test, _}) => {
  test("get version", ({expect}) => {
    let merlin = startMerlin();
    let vnum = String.trim(Merlin.run(~input="", merlin, [|"-vnum"|]));
    expect.string(vnum).toEqual("v3.2.2");
  });
  test("simple type-enclosing case from docs", ({expect}) => {
    let merlin = startMerlin();
    let output =
      String.trim(
        Merlin.run(
          ~input=testFile,
          merlin,
          [|"type-enclosing", "-position \"2:5\"", "-filename test.ml"|],
        ),
      );
    let json = Yojson.Safe.from_string(output);
    let res =
      Yojson.Safe.Util.member("value", json)
      |> Merlin.typeEnclosingResult_of_yojson
      |> Protocol.Utility.getResultOrThrow;
    expect.string(List.hd(res).enclosingType).toEqual("int");
  });
});
