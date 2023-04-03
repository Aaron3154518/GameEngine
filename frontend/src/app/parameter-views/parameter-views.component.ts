import {
  Component,
  DoCheck,
  Input,
  IterableDiffer,
  IterableDiffers,
  OnChanges,
  OnInit,
  Pipe,
  PipeTransform,
  SimpleChanges,
} from '@angular/core';
import { ParameterGroup, Parameters } from '../utils/interfaces';
import { ParameterService } from '../services/parameter.service';
import { ParameterDragService } from '../services/parameter-drag.service';
import { sanitizeVar } from '../utils/utils';
import { InputComponent } from '../search/input/input.component';

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
  dataGroup = ParameterDragService.DataType.Group;
  srcGroup = ParameterDragService.SrcType.Group;

  constructor(protected parameterDragService: ParameterDragService) {}
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
interface Variable {
  name: string;
  id: string;
}

@Component({
  selector: 'var-list',
  templateUrl: './templates/var-list.component.html',
})
export class VarListComponent implements ColComponent, OnInit, OnChanges {
  @Input() row: ParameterGroup | Parameters = new ParameterGroup();
  @Input() type: ParameterDragService.DataType =
    ParameterDragService.DataType.Var;

  @Input() input: boolean = true;
  @Input() draggable: boolean = true;

  srcType: ParameterDragService.SrcType = ParameterDragService.SrcType.Group;
  group: ParameterGroup = new ParameterGroup();
  set: Parameters | undefined;
  vals: Variable[] = [];

  readonly classes: string[] = [
    'py-0',
    'px-1',
    'me-1',
    'rounded-1',
    'border',
    'border-top-0',
    'border-bottom-0',
    'border-secondary',
    'fw-bold',
    'bg-transparent',
  ];

  sanitizeVar = sanitizeVar;

  protected iterableDiffer: IterableDiffer<any>;

  get groups(): Set<string> {
    return this.row instanceof Parameters ? this.row.groups : new Set<string>();
  }

  constructor(
    protected parameterService: ParameterService,
    protected parameterDragService: ParameterDragService,
    iterableDiffers: IterableDiffers
  ) {
    this.iterableDiffer = iterableDiffers.find([]).create(undefined);
  }

  ngOnInit(): void {
    this.computeVals();

    this.row.$changes.subscribe(() => this.computeVals());
  }

  ngOnChanges(changes: SimpleChanges): void {
    this.computeVals();

    this.row.$changes.subscribe(() => this.computeVals());
  }

  computeVals() {
    this.srcType =
      this.row instanceof ParameterGroup
        ? ParameterDragService.SrcType.Group
        : ParameterDragService.SrcType.Set;
    this.group = this.row instanceof ParameterGroup ? this.row : this.row.group;
    this.set = this.row instanceof Parameters ? this.row : undefined;
    this.vals =
      this.type === ParameterDragService.DataType.Var
        ? [...this.group.params].sort().map((s: string) => ({ name: s, id: s }))
        : this.set
        ? [...this.set.groups]
            .map((uuid: string) => this.parameterService.getParamGroup(uuid))
            .reduce((arr: Variable[], g?: ParameterGroup) => {
              if (g) {
                arr.push({ name: g.name, id: g.uuid });
              }
              return arr;
            }, [])
            .sort((a: Variable, b: Variable) =>
              a.name < b.name ? -1 : b.name < a.name ? 1 : 0
            )
        : [];
  }

  onEnter(input: InputComponent) {
    this.row.addParam(input.value);
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
