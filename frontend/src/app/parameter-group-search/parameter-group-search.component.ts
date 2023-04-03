import { Component } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { ParameterGroup, StringDict } from '../utils/interfaces';
import { alphanum_, searchScore, sortList, validateVar } from '../utils/utils';
import { Column } from '../search/search.component';
import {
  GroupColHeaderComponent,
  VarListComponent,
} from '../parameter-views/parameter-views.component';
import { ParameterDragService } from '../services/parameter-drag.service';

@Component({
  selector: 'app-parameter-group-search',
  templateUrl: './parameter-group-search.component.html',
  styleUrls: ['./parameter-group-search.component.css'],
})
export class ParameterGroupSearchComponent {
  cols: Column[] = [
    new Column({
      key: 'name',
      width: 0,
      requireInput: true,
      inputPasslist: alphanum_,
      inputPlaceholder: 'New: Group',
      validateInput: validateVar,
      component: GroupColHeaderComponent,
    }),
    new Column({
      key: 'params',
      component: VarListComponent,
    }),
  ];

  constructor(
    protected parameterService: ParameterService,
    protected parameterDragService: ParameterDragService
  ) {}

  sortParamGroups(rows: ParameterGroup[], query: string) {
    sortList(
      rows,
      searchScore(query, (pg: ParameterGroup) => [pg.name, ...pg.params])
    );
  }

  newGroup(args: StringDict<string>) {
    this.parameterService.newParamGroup(
      new ParameterGroup({ name: args['name'] })
    );
  }
}
