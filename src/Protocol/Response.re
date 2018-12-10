/* module Types = Types; */

[@deriving yojson({strict: false})]
type serverCapabilities = {textDocumentSync: int};

[@deriving yojson({strict: false})]
type initializeResult = {capabilities: serverCapabilities};
