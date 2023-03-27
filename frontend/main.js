const { app, BrowserWindow } = require("electron");

app.whenReady().then(() => {
  let win = new BrowserWindow({ width: 800, height: 800 });
  win.loadFile("dist/frontend/index.html");
});
