open TestFramework;

module Utility = Protocol.Utility;

describe("Utility", ({describe, _}) =>
  describe("uriToPath", ({test, _}) => {
    test("parses Windows paths", ({expect}) => {
      let testPath = "file:///C:/test/myFile.txt";
      expect.string(Utility.uriToPath(~isWindows=true, testPath)).toEqual(
        "C:/test/myFile.txt",
      );
    });

    test("parses non-Windows paths", ({expect}) => {
      let testPath = "file:///test/myFile.txt";
      expect.string(Utility.uriToPath(~isWindows=false, testPath)).toEqual(
        "/test/myFile.txt",
      );
    });
  })
);
