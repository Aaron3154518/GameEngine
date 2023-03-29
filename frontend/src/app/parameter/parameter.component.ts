import { Component, ElementRef, Input, OnInit, ViewChild } from '@angular/core';
import { Callback, CodeType } from '../utils/interfaces';

@Component({
  selector: 'app-parameter',
  templateUrl: './parameter.component.html',
  styleUrls: ['./parameter.component.css'],
})
export class ParameterComponent implements OnInit {
  @Input() callback: Callback = new Callback();
  code: string = '';
  name: string = '';
  idxs: number[][] = [[0, 0]];

  CodeType = CodeType;

  ngOnInit(): void {
    this.code = this.callback.code;
  }

  update() {
    if (!this.name || !this.code) {
      return;
    }

    this.callback.code = this.code;
  }

  onCodeChanged(event: Event) {
    this.code = (event.target as HTMLTextAreaElement).value;
    this.idxs = this.getVarIdxs();
  }

  onNameChanged(event: Event) {
    this.name = (event.target as HTMLInputElement).value;
  }

  select(pid: string, name: string) {
    console.log('Show Parameters:', pid, name);
  }

  isNameUsed(): boolean {
    return false;
  }

  getVarIdxs(): number[][] {
    let var_name: string = Object.values(
      Object.values(this.callback.params)[0]
    )[0];
    let lines: string[] = this.code.split('\n');
    let global_idx: number = 0;
    return lines.map((l: string) => {
      let idx: number = l.indexOf(var_name);
      //let i: number = this.code.search(`${var_name}|\n`);
      let idxs: number[] = [global_idx];
      while (idx !== -1) {
        idxs.push(global_idx + idx);
        idx += var_name.length;
        idxs.push(global_idx + idx);
        idx = l.indexOf(var_name, idx);
      }
      global_idx += l.length;
      idxs.push(global_idx);
      global_idx += 1;
      return idxs;
    });
  }
}
