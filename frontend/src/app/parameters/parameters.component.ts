import { Component, OnInit } from '@angular/core';
import {
  newParamList,
  Param,
  ParameterService,
  ParamInfo,
  ParamList,
} from '../services/parameter.service';

@Component({
  selector: 'app-parameters',
  templateUrl: './parameters.component.html',
  styleUrls: ['./parameters.component.css'],
})
export class ParametersComponent implements OnInit {
  paramLists: ParamList[] = [];
  currParam: ParamInfo = new ParamInfo(newParamList(), 0);

  constructor(private parameterService: ParameterService) {}

  ngOnInit(): void {
    this.parameterService.$paramLists.subscribe(
      (paramLists: ParamList[]) => (this.paramLists = paramLists)
    );

    this.selectParam(this.paramLists[0], 0);
  }

  selectParam(list: ParamList, i: number) {
    this.currParam = new ParamInfo(list, i);
  }
}
export { Param };
