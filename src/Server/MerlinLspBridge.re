/*
 * MerlinLspBridge
 *
 * An interface between the LSP protocol <-> Merlin
 */

/* open Rench; */

module LspProtocol = Protocol;

let _convertLspPositionToMerlinPosition: LspProtocol.Types.position => Merlin.Position.t = (lspPosition) => {
    let pos: Merlin.Position.t = {
        line: lspPosition.line + 1,
        col: lspPosition.character + 1,
    };
    pos;
};

let _convertMerlinPositionToLspPosition: Merlin.Position.t => LspProtocol.Types.position = (merlinPosition) => {
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
}

let hover = (merlin: Merlin.t, store: DocumentStore.t, textDocumentPosition: LspProtocol.Types.textDocumentPositionParams) => {

    let uri = textDocumentPosition.textDocument.uri;
    let fileContents = switch(DocumentStore.getDocument(store, uri)) {
    | Some(v) => v.text
    | None => ""
    };

    let merlinPosition = textDocumentPosition.position |> _convertLspPositionToMerlinPosition;
    let merlinFile = uri |> LspProtocol.Utility.uriToPath;

    let typeEnclosing = Merlin.getTypeEnclosing(merlin, merlinPosition, merlinFile, fileContents);

    switch (typeEnclosing) {
    | Ok(v) => {
        switch (_getFirstEnclosingType(v)) {
        | Some(t) =>
            let ret: LspProtocol.Response.hover = {contents: t};    
            Some(ret);
        | None => None;
        }
        
    }
    | _ => None;
    };
};

let merlinErrorToLspError: Merlin.Protocol.errorResultItem => LspProtocol.Types.diagnostic = (merlinError) => {
    let range = LspProtocol.Types.Range.create(
        _convertMerlinPositionToLspPosition(merlinError.startPosition),
        _convertMerlinPositionToLspPosition(merlinError.endPosition),
    );

    let ret: LspProtocol.Types.diagnostic = {
        message: merlinError.message,
        range,
        severity: 1,
    };

    ret;
}

let errors = (merlin: Merlin.t, store: DocumentStore.t, uri: LspProtocol.Types.documentUri) => {

    switch(DocumentStore.getDocument(store, uri)) {
    | None => None
    | Some(v) => {
       let merlinFile = uri |> LspProtocol.Utility.uriToPath; 
       let errors = Merlin.getErrors(merlin, merlinFile, v.text);

       switch(errors) {
       | Error(msg) => 
            prerr_endline ("ERROR: " ++ msg);
            None
       | Ok(e) => {
            List.map(merlinErrorToLspError, e)
            |> Some;
       }

       }
    }
    };
    
};
