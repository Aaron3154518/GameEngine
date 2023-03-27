const { app, BrowserWindow } = require("electron");

app.whenReady().then(() => {
  let win = new BrowserWindow({
    width: 800,
    height: 800,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
      preload: __dirname + "\\src\\preload.js",
    },
  });
  win.loadFile("dist/frontend/index.html");
  win.webContents.openDevTools();
});
