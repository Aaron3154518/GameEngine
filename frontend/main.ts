import { app, BrowserWindow } from "electron";
import Utils from "./utils";

export default class Main {
  win: Electron.BrowserWindow;

  private onWindowAllClosed() {
    if (process.platform !== "darwin") {
      app.quit();
    }
  }

  private onClose() {
    this.win = null;
  }

  private onReady() {
    this.win = new BrowserWindow({ width: 800, height: 600 });
    this.win.loadFile(Utils.file("main.html"));
    this.win.on("closed", () => this.onClose());
  }

  constructor() {
    app.on("window-all-closed", () => this.onWindowAllClosed());
    app.on("ready", () => this.onReady());
  }
}

export function refresh() {
  console.log("Refresh");
}
