import { Component } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import {
  sanitizeTypeVar,
  searchScore,
  sortList,
  validateTypeVar,
} from '../utils/utils';
import { Parameters, StringDict } from '../utils/interfaces';
import { Column } from '../search/search.component';
import {
  GroupListComponent,
  SetColHeaderComponent,
  VarListComponent,
} from '../parameter-views/parameter-views.component';

@Component({
  selector: 'app-parameters-search',
  templateUrl: './parameters-search.component.html',
  styleUrls: ['./parameters-search.component.css'],
})
export class ParametersSearchComponent {
  cols: Column[] = [
    new Column({
      key: 'type',
      getter: (set: Parameters) => ({ type: set.type, name: set.name }),
      width: 0,
      requireInput: true,
      inputClasses: ['type'],
      inputPlaceholder: 'New: Type Name',
      validateInput: validateTypeVar,
      sanitizeInput: sanitizeTypeVar,
      component: SetColHeaderComponent,
    }),
    new Column({
      key: 'group.params',
      width: 2,
      component: VarListComponent,
    }),
    new Column({
      key: 'groups',
      width: 1,
      cellClasses: ['border-start'],
      component: GroupListComponent,
    }),
  ];

  constructor(protected parameterService: ParameterService) {}

  sortParamSets(rows: Parameters[], query: string) {
    sortList(
      rows,
      searchScore(query, (set: Parameters) => [set.name, set.type])
    );
  }

  newSet(args: StringDict<string>) {
    this.parameterService.newParamSet(
      new Parameters({
        type: args['type'],
        name: args['name'],
      })
    );
  }
}
