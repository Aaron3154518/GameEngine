import {
  Component,
  EventEmitter,
  Input,
  Output,
  Pipe,
  PipeTransform,
} from '@angular/core';
import { ParameterGroup, Parameters } from '../utils/interfaces';
import { ParameterService } from '../services/parameter.service';
import { ParameterDragService } from '../services/parameter-drag.service';
import { sanitizeVar } from '../utils/utils';
import { InputComponent } from '../search/input/input.component';

@Pipe({
  name: 'group_names',
  pure: false,
})
export class GroupNamesPipe implements PipeTransform {
  transform(
    items: Set<string>,
    parameterService: ParameterService
  ): Set<string> {
    return [...items].reduce((set: Set<string>, id: string) => {
      let group: ParameterGroup | undefined =
        parameterService.getParamGroup(id);
      if (group && group.params.size > 0) {
        set.add(group.name);
      }
      return set;
    }, new Set<string>());
  }
}

export interface ColComponent {
  row?: any;
  value?: any;
}

// Column headers
const colHeaderClasses: string[] = [
  'input-group-text',
  'd-inline-block',
  'text-start',
  'py-0',
  'px-1',
  'w-100',
];

@Component({
  selector: 'group-col-header',
  templateUrl: './templates/group-col-header.component.html',
})
export class GroupColHeaderComponent implements ColComponent {
  @Input() row: ParameterGroup = new ParameterGroup();
  @Input() value: string = '';

  classes: string[] = colHeaderClasses;

  constructor(protected parameterDragService: ParameterDragService) {}

  onDragStart(event: DragEvent) {
    this.parameterDragService.onDragStart(event, {
      type: ParameterDragService.DataType.Group,
      value: this.row.uuid,
    });
  }
}

@Component({
  selector: 'type-col-header',
  templateUrl: './templates/col-header.component.html',
})
export class TypeColHeaderComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: string = '';

  classes: string[] = colHeaderClasses.concat([
    'type',
    'border-end-0',
    'rounded-0',
    'rounded-start',
    'text-end',
  ]);

  constructor(protected parameterDragService: ParameterDragService) {}
}

@Component({
  selector: 'name-col-header',
  templateUrl: './templates/col-header.component.html',
})
export class NameColHeaderComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: string = '';

  classes: string[] = colHeaderClasses.concat([
    'border-start-0',
    'rounded-0',
    'rounded-end',
  ]);

  constructor(protected parameterDragService: ParameterDragService) {}
}

// Variable lists
@Component({
  selector: 'var-list',
  templateUrl: './templates/var-list.component.html',
})
export class VarListComponent implements ColComponent {
  @Input() value: Set<string> = new Set<string>();
  @Input() set row(p: ParameterGroup | Parameters) {
    this._row = p instanceof ParameterGroup ? p : p.group;
  }
  @Input() uuid: string = '';
  @Input() type: ParameterDragService.DataType =
    ParameterDragService.DataType.Var;

  @Input() input: boolean = true;
  @Input() draggable: boolean = true;
  @Input() inline: boolean = false;

  get row(): ParameterGroup {
    return this._row;
  }

  private _row: ParameterGroup = new ParameterGroup();

  readonly classes: string[] = [
    'py-0',
    'px-1',
    'mx-1',
    'rounded-1',
    'border',
    'border-top-0',
    'border-bottom-0',
    'border-secondary',
    'fw-bold',
    'bg-transparent',
  ];

  sanitizeVar = sanitizeVar;

  constructor(protected parameterDragService: ParameterDragService) {}

  onEnter(input: InputComponent) {
    this.row?.addParam(input.value);
    input.value = '';
  }
}

@Component({
  selector: 'group-list',
  templateUrl: './templates/group-list.component.html',
})
export class GroupListComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: Set<string> = new Set<string>();

  Group = ParameterDragService.DataType.Group;

  constructor(
    protected parameterService: ParameterService,
    protected parameterDragService: ParameterDragService
  ) {}
}
