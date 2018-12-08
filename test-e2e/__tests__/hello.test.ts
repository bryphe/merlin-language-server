import * as cp from 'child_process';
import * as os from 'os';
import * as path from 'path';
import * as rpc from 'vscode-jsonrpc'

let serverRootPath = path.join(__dirname, "..", "..", "_build", "install", "default", "bin");
let serverBin = os.platform() === "win32" ? "Hello.exe" : "Hello";
let serverPath = path.join(serverRootPath, serverBin);

test('basic', () => {
    expect(1).toBe(1);

    let childProcess = cp.spawn(serverPath);

    let connection = rpc.createMessageConnection(
        new rpc.StreamMessageReader(childProcess.stdout),
        new rpc.StreamMessageWriter(childProcess.stdin)
    );

    childProcess.stderr.on("data", (d) => {
        console.log("Received data: " + d);
    })


    let ret = new Promise((resolve, reject) => {
        connection.onClose(() => {
            console.log("Closing!");
            resolve();
        })
    })


    let notification = new rpc.NotificationType<string, void>('exit');

    connection.listen();

    connection.sendNotification(notification);


    return ret;

});
