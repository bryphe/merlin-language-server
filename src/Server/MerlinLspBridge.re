/*
 * MerlinLspBridge
 *
 * An interface between the LSP protocol <-> Merlin
 */

module Log = Protocol.Log;
module LspProtocol = Protocol;

let _convertLspPositionToMerlinPosition:
  LspProtocol.Types.position => Merlin.Position.t =
  lspPosition => {
    let pos: Merlin.Position.t = {
      line: lspPosition.line + 1,
      col: lspPosition.character + 1,
    };
    pos;
  };

let _convertMerlinPositionToLspPosition:
  Merlin.Position.t => LspProtocol.Types.position =
  merlinPosition => {
    let pos: LspProtocol.Types.position = {
      line: merlinPosition.line - 1,
      character: merlinPosition.col,
    };
    pos;
  };

let _getFirstEnclosingType = (types: Merlin.Protocol.typeEnclosingResult) => {
  switch (types) {
  | [] => None
  | [hd] => Some(hd.enclosingType)
  | [hd, ..._tl] => Some(hd.enclosingType)
  };
};

let hover =
    (
      merlin: Merlin.t,
      store: DocumentStore.t,
      textDocumentPosition: LspProtocol.Types.textDocumentPositionParams,
    ) => {
  let uri = textDocumentPosition.textDocument.uri;
  let fileContents =
    switch (DocumentStore.getDocument(store, uri)) {
    | Some(v) => v.text
    | None => ""
    };

  let merlinPosition =
    textDocumentPosition.position |> _convertLspPositionToMerlinPosition;
  let merlinFile = uri |> LspProtocol.Utility.uriToPath;

  let typeEnclosing =
    Merlin.getTypeEnclosing(merlin, merlinPosition, merlinFile, fileContents);

  switch (typeEnclosing) {
  | Ok(v) =>
    switch (_getFirstEnclosingType(v)) {
    | Some(t) =>
      let ret: LspProtocol.Response.hover = {contents: t};
      Some(ret);
    | None => None
    }
  | _ => None
  };
};

let merlinErrorToLspError:
  Merlin.Protocol.errorResultItem => LspProtocol.Types.diagnostic =
  merlinError => {
    let range =
      LspProtocol.Types.Range.create(
        _convertMerlinPositionToLspPosition(merlinError.startPosition),
        _convertMerlinPositionToLspPosition(merlinError.endPosition),
      );

    let ret: LspProtocol.Types.diagnostic = {
      message: merlinError.message,
      range,
      severity: 1,
    };

    ret;
  };

let errors =
    (
      merlin: Merlin.t,
      store: DocumentStore.t,
      uri: LspProtocol.Types.documentUri,
    ) => {
  switch (DocumentStore.getDocument(store, uri)) {
  | None => None
  | Some(v) =>
    let merlinFile = uri |> LspProtocol.Utility.uriToPath;
    let errors = Merlin.getErrors(merlin, merlinFile, v.text);

    switch (errors) {
    | Error(msg) =>
      LspProtocol.Log.error(msg);
      None;
    | Ok(e) => List.map(merlinErrorToLspError, e) |> Some
    };
  };
};

let _merlinCompletionToLspCompletion =
    (merlinEntry: Merlin.Protocol.completionResultItem) => {
  let completionItem: LspProtocol.Response.completionItem = {
    label: merlinEntry.name,
    detail: merlinEntry.desc,
  };
  completionItem;
};

let _merlinCompletionsToLspCompletions =
    (merlinEntries: list(Merlin.Protocol.completionResultItem)) => {
  List.map(_merlinCompletionToLspCompletion, merlinEntries);
};

let completion =
    (
      merlin: Merlin.t,
      store: DocumentStore.t,
      textDocumentPosition: LspProtocol.Types.textDocumentPositionParams,
    ) => {
  let uri = textDocumentPosition.textDocument.uri;
  let fileContents =
    switch (DocumentStore.getDocument(store, uri)) {
    | Some(v) => v.text
    | None => ""
    };

  let merlinPosition =
    textDocumentPosition.position |> _convertLspPositionToMerlinPosition;
  let merlinFile = uri |> LspProtocol.Utility.uriToPath;

  switch (DocumentStore.getTokenAt(store, uri, textDocumentPosition.position)) {
  | None => None
  | Some(v) =>
    Log.verbose("MerlinLspBridge::completion - using prefix: |" ++ v ++ "|");
    let completions =
      Merlin.getCompletePrefix(
        merlin,
        v,
        merlinPosition,
        merlinFile,
        fileContents,
      );

    switch (completions) {
    | Error(msg) =>
      Log.error(msg);
      None;
    | Ok(v) =>
      let completions = _merlinCompletionsToLspCompletions(v);
      let completionList: LspProtocol.Response.completionList = {
        isIncomplete: true,
        items: completions,
      };
      Some(completionList);
    };
  };
};
