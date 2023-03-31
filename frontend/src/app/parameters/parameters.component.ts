import { Component } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { Callback } from '../utils/interfaces';

@Component({
  selector: 'app-parameters',
  templateUrl: './parameters.component.html',
  styleUrls: ['./parameters.component.css'],
})
export class ParametersComponent {
  currCallback?: Callback;

  constructor(protected parameterService: ParameterService) {}

  selectCallback(cb: Callback) {
    this.currCallback = cb;
  }

  codegen() {
    this.parameterService.codegen();
  }
}
