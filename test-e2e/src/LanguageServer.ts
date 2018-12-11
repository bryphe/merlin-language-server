import * as cp from 'child_process';
import * as os from 'os';
import * as path from 'path';
import * as rpc from 'vscode-jsonrpc'

import * as Protocol from "vscode-languageserver-protocol"
import * as Rpc from "vscode-jsonrpc"

let serverRootPath = path.join(__dirname, "..", "..", "_build", "install", "default", "bin");
let serverBin = os.platform() === "win32" ? "merlin-language-server.exe" : "merlin-language-server";
let serverPath = path.join(serverRootPath, serverBin);

export type LanguageServer = Rpc.MessageConnection;

export const start = (opts?: cp.SpawnOptions) => {
    opts = opts || {};
    let childProcess = cp.spawn(serverPath, opts);

    let connection = rpc.createMessageConnection(
        new rpc.StreamMessageReader(childProcess.stdout),
        new rpc.StreamMessageWriter(childProcess.stdin)
    );

    childProcess.stderr.on("data", (d) => {
        console.log("Received data: " + d);
    })

    connection.listen();

    return connection as LanguageServer;
};

export const startAndInitialize = async (opts?: cp.SpawnOptions) => {
    let languageServer = start(opts);

    let capabilities: Protocol.ClientCapabilities = { };

    let initializeParameters: Protocol.InitializeParams = {
        processId: process.pid,
        rootUri: process.cwd(),
        capabilities: capabilities,
        workspaceFolders: [],
    };

    await languageServer.sendRequest(Protocol.InitializeRequest.type, initializeParameters);
    return languageServer;
};

export const exit = async (languageServer) => {
    let ret = new Promise((resolve, reject) => {
        languageServer.onClose(() => {
            console.log("Closing!");
            resolve();
        })
    })

    let notification = new rpc.NotificationType<string, void>('exit');
    languageServer.sendNotification(notification);

    return ret;
};
