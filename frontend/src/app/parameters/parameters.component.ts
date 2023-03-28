import { Component, OnInit } from '@angular/core';
import { MdbModalService } from 'mdb-angular-ui-kit/modal';
import {
  newParamList,
  Param,
  ParameterService,
  ParamInfo,
  ParamList,
} from '../services/parameter.service';
import { TypeModalComponent } from '../type-modal/type-modal.component';
import { searchScore, sortListAscending } from '../utils';

@Component({
  selector: 'app-parameters',
  templateUrl: './parameters.component.html',
  styleUrls: ['./parameters.component.css'],
})
export class ParametersComponent implements OnInit {
  _paramLists: ParamList[] = [];
  _search: string = '';
  currParam: ParamInfo = new ParamInfo(newParamList(), 0);

  constructor(
    private parameterService: ParameterService,
    private modalService: MdbModalService
  ) {}

  get paramLists(): ParamList[] {
    return this._paramLists;
  }

  set paramLists(list: ParamList[]) {
    this._paramLists = [...list];
    this.sortParamLists();
  }

  get search(): string {
    return this._search;
  }

  set search(text: string) {
    this._search = text;
    this.sortParamLists();
  }

  ngOnInit(): void {
    this.parameterService.$paramLists.subscribe(
      (paramLists: ParamList[]) => (this.paramLists = paramLists)
    );

    this.selectParam(this.paramLists[0], 0);
  }

  selectParam(list: ParamList, i: number) {
    this.currParam = new ParamInfo(list, i);
  }

  onSearchChanged(event: Event) {
    this.search = (event.target as HTMLInputElement).value;
  }

  sortParamLists() {
    sortListAscending(
      this.paramLists,
      searchScore(this.search, ['name', 'type'])
    );
  }

  listExists(name: string) {
    return this.paramLists.findIndex((l: ParamList) => l.name === name) !== -1;
  }

  createParamList() {
    this.modalService.open(TypeModalComponent, {
      data: { name: this.search },
    });
  }

  codegen() {
    this.parameterService.codegen();
  }
}
export { Param };
