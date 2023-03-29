import { Component, OnInit } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { ParameterGroup } from '../utils/interfaces';
import { searchScore, sortListAscending } from '../utils/utils';

@Component({
  selector: 'app-enum-search',
  templateUrl: './enum-search.component.html',
  styleUrls: ['./enum-search.component.css'],
})
export class ParameterGroupSearchComponent implements OnInit {
  _groups: ParameterGroup[] = [];
  _search: string = '';

  constructor(private parameterService: ParameterService) {}

  get groups(): ParameterGroup[] {
    return this._groups;
  }

  set groups(list: ParameterGroup[]) {
    this._groups = [...list];
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
    this.parameterService.$paramGroupsChanged.subscribe(
      () => (this.groups = [...this.parameterService.paramGroups])
    );
  }

  onSearchChanged(event: Event) {
    this.search = (event.target as HTMLInputElement).value;
  }

  createParameterGroup() {}

  sortParamLists() {
    sortListAscending(this.groups, searchScore(this.search, ['name']));
  }
}
