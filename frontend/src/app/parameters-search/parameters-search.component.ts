import { Component, OnInit } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { searchScore, sortListAscending } from '../utils/utils';
import { Parameters } from '../utils/interfaces';

// TODO: delete type-modal

@Component({
  selector: 'app-parameters-search',
  templateUrl: './parameters-search.component.html',
  styleUrls: ['./parameters-search.component.css'],
})
export class ParametersSearchComponent implements OnInit {
  _parameters: Parameters[] = [];
  _search: string = '';

  constructor(protected parameterService: ParameterService) {}

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
}
