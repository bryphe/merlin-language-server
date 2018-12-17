/* Common types described by the LSP */

[@deriving yojson({strict: false})]
type documentUri = string;

[@deriving yojson({strict: false})]
type textDocumentItem = {
  uri: documentUri,
  languageId: string,
  /* version: int, */
  text: string,
};

[@deriving yojson({strict: false})]
type versionedTextDocumentIdentifier = {
  uri: documentUri,
  version: option(int),
};

/* TODO: Change strict to false later */
[@deriving yojson({strict: true})]
type textDocumentContentChangeEvent = {
  /* TODO: range */
  /* TODO: rangeLength */
  text: string,
};

[@deriving yojson({strict: false})]
type zeroBasedLine = int;

[@deriving yojson({strict: false})]
type zeroBasedCharacter = int;

module Position = {
  [@deriving yojson({strict: false})]
  type t = {
    line: zeroBasedLine,
    character: zeroBasedCharacter,
  };

  let create = (line: zeroBasedLine, character: zeroBasedCharacter) => {
    let ret: t = {line, character};
    ret;
  };
};

/* TODO: Refactor */
[@deriving yojson({strict: false})]
type position = Position.t;

module Range = {
  [@deriving yojson({strict: false})]
  type t = {
    [@key "start"]
    startPosition: position,
    [@key "end"]
    endPosition: position,
  };

  let create = (startPosition, endPosition) => {
    let ret: t = {startPosition, endPosition};
    ret;
  };
};

[@deriving yojson({strict: false})]
type textDocumentIdentifier = {uri: documentUri};

[@deriving yojson({strict: false})]
type textDocumentPositionParams = {
  textDocument: textDocumentIdentifier,
  position,
};

/* Types used for debugging */
[@deriving yojson({strict: false})]
type debugEchoParams = {message: string};

[@deriving yojson({strict: false})]
type diagnostic = {
  range: Range.t,
  message: string,
  severity: int,
};

[@deriving yojson({strict: false})]
type diagnostics = list(diagnostic);
