import { Component } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import {
  sanitizeType,
  sanitizeVar,
  searchScore,
  sortList,
  validateType,
  validateVar,
} from '../utils/utils';
import { Parameters, StringDict } from '../utils/interfaces';
import { Column } from '../search/search.component';
import {
  GroupListComponent,
  NameColHeaderComponent,
  TypeColHeaderComponent,
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
      width: 0,
      requireInput: true,
      inputClasses: ['float-end', 'type'],
      inputPlaceholder: 'New: Type',
      validateInput: validateType,
      sanitizeInput: sanitizeType,
      component: TypeColHeaderComponent,
    }),
    new Column({
      key: 'name',
      width: 0,
      requireInput: true,
      inputPlaceholder: 'Name',
      validateInput: (s: string): boolean =>
        validateVar(s) &&
        this.parameterService.paramSets.findIndex(
          (set: Parameters) => set.name === s
        ) === -1,
      sanitizeInput: sanitizeVar,
      component: NameColHeaderComponent,
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
