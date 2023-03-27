import { Component, Input, OnInit } from '@angular/core';
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

  ngOnInit(): void {}

  update() {
    if (!this.name || !this.code) {
      return;
    }

    this.param.param.callbacks[this.name] = this.code;
  }

  onCodeChanged(event: Event) {
    this.code = (event.target as HTMLTextAreaElement).value;
  }

  onNameChanged(event: Event) {
    this.name = (event.target as HTMLInputElement).value;
  }

  isNameUsed(): boolean {
    return this.name in this.param.param.callbacks;
  }
}
