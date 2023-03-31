import { Component, OnInit } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { MdbModalService } from 'mdb-angular-ui-kit/modal';
import { searchScore, sortListAscending } from '../utils/utils';
import { TypeModalComponent } from '../type-modal/type-modal.component';
import { Parameters } from '../utils/interfaces';

@Component({
  selector: 'app-parameters-search',
  templateUrl: './parameters-search.component.html',
  styleUrls: ['./parameters-search.component.css'],
})
export class ParametersSearchComponent implements OnInit {
  _parameters: Parameters[] = [];
  _search: string = '';

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
}
