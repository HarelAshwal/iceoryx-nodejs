const iceoryx = require('bindings')('iceoryx.node')

iceoryx.setIoxProcessName("client_test");

const client = new iceoryx.IceoryxClient();

let resp = client.sendMessage("{koko : 10}");
console.log("got some resp:" + resp)


