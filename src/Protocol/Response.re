/* module Types = Types; */

[@deriving yojson({strict: false})]
type serverCapabilities = {textDocumentSync: int};

[@deriving yojson({strict: false})]
type initializeResult = {capabilities: serverCapabilities};

[@deriving yojson({strict: false})]
type hover = {contents: string};

[@deriving yojson({strict: false})]
type completionItem = {
  label: string,
  detail: string,
};

[@deriving yojson({strict: false})]
type completionList = {
  isIncomplete: bool,
  items: list(completionItem),
};

type response =
  | UnknownResponse;

type t = response;

let is = (msg: Yojson.Safe.json) =>
  Utility.hasResult(msg) && Utility.hasId(msg);
