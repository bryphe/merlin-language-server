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

    prerr_endline ("--- 1!: file contents: " ++ fileContents);

    let merlinPosition = textDocumentPosition.position |> _convertLspPositionToMerlinPosition;
    let merlinFile = uri |> LspProtocol.Utility.uriToPath;

    let typeEnclosing = Merlin.getTypeEnclosing(merlin, merlinPosition, merlinFile, fileContents);

    switch (typeEnclosing) {
    | Ok(v) => {
        switch (_getFirstEnclosingType(v)) {
        | Some(t) =>
            let ret: LspProtocol.Response.hover = {contents: t};    
            prerr_endline("--- 2!: got hover");
            Some(ret);
        | None => None;
        }
        
    }
    | _ => None;
    };
};
