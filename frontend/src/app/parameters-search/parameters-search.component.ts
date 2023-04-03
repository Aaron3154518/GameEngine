import { Component } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import {
  alphanum_,
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
      key: 'type var',
      getter: (set: Parameters) => ({ type: set.type, name: set.name }),
      width: 0,
      requireInput: true,
      inputPasslist: `${alphanum_}: `,
      inputMaxlen: 30,
      inputClasses: ['type'],
      inputPlaceholder: 'New: Type Name',
      validateInput: (s: string) =>
        validateTypeVar(s) &&
        !this.parameterService.paramSetNameExists(s.split(' ')[1]),
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
    let split: string[] = args['type var'].split(' ');
    this.parameterService.newParamSet(
      new Parameters({
        type: split[0],
        name: split[1],
      })
    );
  }
}
