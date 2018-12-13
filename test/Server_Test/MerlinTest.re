open TestFramework;

module Merlin = Server.Merlin;

let startMerlin = () => {
  let merlinPath = Rench.Environment.which("ocamlmerlin");
  switch (merlinPath) {
  | Some(v) => Merlin.init(v)
  | None => Merlin.init("ocamlmerlin")
  };
};

/* Sample case from:
 * https://github.com/ocaml/merlin/blob/master/doc/dev/PROTOCOL.md
 */

let testFile = {|
let x = 5
let y = 3.0 *. x
|};

describe("Merlin", ({describe, _}) => {
  describe("getTypeEnclosing", ({test, _}) => {
      test("success case: simple type-enclosing case from docs", ({expect}) => {
        let merlin = startMerlin();

        let result =
          Merlin.getTypeEnclosing(
            merlin,
            {line: 2, col: 5},
            "test.ml",
            testFile,
          );
        switch (result) {
        | Ok(r) => expect.string(List.hd(r).enclosingType).toEqual("int")
        | _ => expect.bool(false).toBe(true)
        };
      });
      test("failure case: type-enclosing case from docs", ({expect}) => {
        let merlin = startMerlin();

        let result =
          Merlin.getTypeEnclosing(
            merlin,
            {line: 0, col: 0},
            "test.ml",
            testFile,
          );
        switch (result) {
        | Error(_) => expect.bool(true).toBe(true)
        | Ok(_) => expect.bool(false).toBe(true)
        };
      });
  });
  describe("getCompletePrefix", ({test, _}) => {
      test("success case: simple complete-prefix closing case from docs", ({expect}) => {
        let merlin = startMerlin();

        let result =
          Merlin.getCompletePrefix(
            merlin,
            "List.m",
            {line: 3, col: 14},
            "test.ml",
            testFile,
          );
        switch (result) {
        | Ok(r) => expect.string(List.hd(r).name).toEqual("map")
        | _ => expect.bool(false).toBe(true)
        };
      });
  });
});
