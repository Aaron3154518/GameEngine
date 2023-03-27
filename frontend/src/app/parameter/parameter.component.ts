import { Component, ElementRef, Input, OnInit, ViewChild } from '@angular/core';
import {
  newParam,
  newParamList,
  Param,
  ParamInfo,
} from '../services/parameter.service';

@Component({
  selector: 'app-parameter',
  templateUrl: './parameter.component.html',
  styleUrls: ['./parameter.component.css'],
})
export class ParameterComponent implements OnInit {
  @Input() param: ParamInfo = new ParamInfo(newParamList(), 0);
  code: string = '';
  name: string = '';
  idxs: number[][] = [[0, 0]];

  ngOnInit(): void {}

  update() {
    if (!this.name || !this.code) {
      return;
    }

    this.param.param.callbacks[this.name] = this.code;
  }

  onCodeChanged(event: Event) {
    this.code = (event.target as HTMLTextAreaElement).value;
    this.idxs = this.getVarIdxs();
  }

  onNameChanged(event: Event) {
    this.name = (event.target as HTMLInputElement).value;
  }

  select(name: string, code: string) {
    this.name = name;
    this.code = code;
    this.idxs = this.getVarIdxs();
  }

  isNameUsed(): boolean {
    return this.name in this.param.param.callbacks;
  }

  getVarIdxs(): number[][] {
    let var_name: string = this.param.name;
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
