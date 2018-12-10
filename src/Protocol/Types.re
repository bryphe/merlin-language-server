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
type zeroBasedLine = int;

[@deriving yojson({strict: false})]
type zeroBasedCharacter = int;

[@deriving yojson({strict: false})]
type position = {
  line: zeroBasedLine,
  character: zeroBasedCharacter,
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
