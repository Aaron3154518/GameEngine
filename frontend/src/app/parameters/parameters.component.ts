import { Component, OnInit } from '@angular/core';
import { MdbModalService } from 'mdb-angular-ui-kit/modal';
import { ParameterService } from '../services/parameter.service';
import { TypeModalComponent } from '../type-modal/type-modal.component';
import { searchScore, sortListAscending } from '../utils/utils';
import { Callback, Parameters } from '../utils/interfaces';

@Component({
  selector: 'app-parameters',
  templateUrl: './parameters.component.html',
  styleUrls: ['./parameters.component.css'],
})
export class ParametersComponent implements OnInit {
  _parameters: Parameters[] = [];
  _search: string = '';
  currCallback?: Callback;

  constructor(
    protected parameterService: ParameterService,
    private modalService: MdbModalService
  ) {}

  get parameters(): Parameters[] {
    return this._parameters;
  }

  set parameters(list: Parameters[]) {
    this._parameters = [...list];
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
    this.parameterService.$paramSetsChanged.subscribe(
      () => (this.parameters = [...this.parameterService.paramSets])
    );
  }

  selectCallback(cb: Callback) {
    this.currCallback = cb;
  }

  onSearchChanged(event: Event) {
    this.search = (event.target as HTMLInputElement).value;
  }

  sortParamLists() {
    sortListAscending(
      this.parameters,
      searchScore(this.search, (set: Parameters) => [set.name, set.type])
    );
  }

  listExists(name: string) {
    return this.parameters.findIndex((l: Parameters) => l.name === name) !== -1;
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
