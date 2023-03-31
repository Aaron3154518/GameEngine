import { Component, ElementRef, Input, OnInit, ViewChild } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { ParameterGroup, StringDict } from '../utils/interfaces';
import { searchScore, sortList } from '../utils/utils';
import {
  ColComponent,
  ColHeaderComponent,
  ColWidth,
  Column,
} from '../search/search.component';

@Component({
  selector: 'app-var',
  template: `
    <span
      *ngFor="let str of value; let first = first"
      class="rounded-1 border border-top-0 border-bottom-0 border-dark py-0 px-1 mx-1 fst-normal"
      >{{ str }}</span
    >
  `,
})
export class VarComponent implements ColComponent {
  @Input() value: Set<string> = new Set<string>();
}

@Component({
  selector: 'app-parameter-group-search',
  templateUrl: './parameter-group-search.component.html',
  styleUrls: ['./parameter-group-search.component.css'],
})
export class ParameterGroupSearchComponent {
  cols: Column[] = [
    new Column({
      key: 'name',
      width: ColWidth.Fit,
      requireInput: true,
      inputPlaceholder: '+ New Group',
      component: ColHeaderComponent,
    }),
    new Column({
      key: 'params',
      component: VarComponent,
      input: this.addParam,
    }),
  ];

  constructor(protected parameterService: ParameterService) {}

  sortParamGroups(rows: ParameterGroup[], query: string) {
    sortList(
      rows,
      searchScore(query, (pg: ParameterGroup) => [pg.name, ...pg.params])
    );
  }

  addParam(group: ParameterGroup, name: string) {
    group.addParam(name);
  }

  newGroup(args: StringDict<string>) {
    this.parameterService.newParamGroup(
      new ParameterGroup({ name: args['name'] })
    );
  }
}
