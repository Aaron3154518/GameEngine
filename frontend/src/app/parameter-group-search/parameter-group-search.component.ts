import { Component, Input } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import { ParameterGroup, StringDict } from '../utils/interfaces';
import { sanitizeVar, searchScore, sortList } from '../utils/utils';
import {
  ColComponent,
  ColHeaderComponent,
  ColWidth,
  Column,
} from '../search/search.component';
import { InputComponent } from '../search/input/input.component';
import { stringify } from 'uuid';

@Component({
  selector: 'group-col-header',
  template: `<col-header
    [value]="value"
    [ngStyle]="{ cursor: row ? 'grab' : 'auto' }"
    [draggable]="row"
    (dragstart)="onDragStart($event)"
  ></col-header>`,
})
export class GroupColHeaderComponent implements ColComponent {
  @Input() row?: ParameterGroup;
  @Input() value: string = '';

  onDragStart(event: DragEvent) {
    event.dataTransfer?.setData('text/plain', this.row ? this.row.uuid : '');
  }
}

@Component({
  selector: 'app-var',
  template: `
    <app-input
      *ngIf="input"
      #colInput
      [classes]="classes"
      placeholder="+ New"
      (enter)="onEnter(colInput)"
      [sanitize]="sanitizeVar"
    >
    </app-input
    ><span
      *ngFor="let str of value; let first = first"
      class="rounded-1 border border-top-0 border-bottom-0 border-dark py-0 px-1 mx-1 fst-normal"
      [ngStyle]="{ cursor: draggable ? 'grab' : 'auto' }"
      [draggable]="draggable"
      (dragstart)="onDragStart($event, str)"
      >{{ str }}</span
    >
  `,
})
export class VarComponent implements ColComponent {
  @Input() row?: ParameterGroup;
  @Input() value: Set<string> = new Set<string>();

  @Input() input: boolean = true;
  @Input() draggable: boolean = true;

  readonly classes: string[] = [
    'py-0',
    'px-1',
    'mx-2',
    'rounded-1',
    'border',
    'border-top-0',
    'border-bottom-0',
    'border-secondary',
    'fw-bold',
  ];

  sanitizeVar = sanitizeVar;

  onDragStart(event: DragEvent, name: string) {
    event.dataTransfer?.setData(
      'text/plain',
      `${this.row ? this.row.uuid : ''} ${name}`
    );
  }

  onEnter(input: InputComponent) {
    this.row?.addParam(input.value);
    input.value = '';
  }
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
      inputPlaceholder: 'New: Group',
      component: GroupColHeaderComponent,
    }),
    new Column({
      key: 'params',
      component: VarComponent,
    }),
  ];

  constructor(protected parameterService: ParameterService) {}

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
