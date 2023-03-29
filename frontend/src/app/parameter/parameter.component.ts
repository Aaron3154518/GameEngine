import { Component, Input, OnChanges, SimpleChanges } from '@angular/core';
import { left } from '@popperjs/core';
import { Callback, CodeType } from '../utils/interfaces';

@Component({
  selector: 'app-parameter',
  templateUrl: './parameter.component.html',
  styleUrls: ['./parameter.component.css'],
})
export class ParameterComponent implements OnChanges {
  @Input() callback: Callback = new Callback();
  code: string = '';
  name: string = '';
  idxs: number[][] = [[0, 0]];

  CodeType = CodeType;

  ngOnChanges(changes: SimpleChanges) {
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
    let vars: string[] = Object.values(this.callback.params).reduce(
      (arr: string[], params: Set<string>) => arr.concat([...params]),
      []
    );
    let regex: RegExp = new RegExp(vars.join('|'));
    let global_idx: number = 0;
    return this.code.split('\n').map((l: string) => {
      let res: RegExpMatchArray | null = l.match(regex);
      let idxs: number[] = [global_idx];
      while (res && res.index !== undefined) {
        let idx: number = res.index;
        idxs.push(global_idx + idx);
        idx += res[0].length;
        idxs.push(global_idx + idx);
        global_idx += idx;
        l = l.slice(idx);
        res = l.match(regex);
      }
      global_idx += l.length;
      idxs.push(global_idx);
      global_idx += 1;
      return idxs;
    });
  }
}
