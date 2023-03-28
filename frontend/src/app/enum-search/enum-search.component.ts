import { Component, OnInit } from '@angular/core';
import { Enum, EnumService } from '../services/enum.service';
import { searchScore, sortListAscending } from '../utils';

@Component({
  selector: 'app-enum-search',
  templateUrl: './enum-search.component.html',
  styleUrls: ['./enum-search.component.css'],
})
export class EnumSearchComponent implements OnInit {
  _enumList: Enum[] = [];
  _search: string = '';

  constructor(private enumService: EnumService) {}

  get enumList(): Enum[] {
    return this._enumList;
  }

  set enumList(list: Enum[]) {
    this._enumList = [...list];
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
    this.enumService.$enumLists.subscribe(
      (enumList: Enum[]) => (this.enumList = enumList)
    );
  }

  onSearchChanged(event: Event) {
    this.search = (event.target as HTMLInputElement).value;
  }

  createEnum() {}

  sortParamLists() {
    sortListAscending(this.enumList, searchScore(this.search, ['name']));
  }
}
