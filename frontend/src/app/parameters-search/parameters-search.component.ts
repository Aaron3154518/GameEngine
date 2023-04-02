import { Component, Injectable, Input } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import {
  sanitizeType,
  sanitizeVar,
  searchScore,
  sortList,
  validateType,
  validateVar,
} from '../utils/utils';
import { ParameterGroup, Parameters, StringDict } from '../utils/interfaces';
import { ColComponent, Column } from '../search/search.component';
import { Pipe, PipeTransform } from '@angular/core';
import { VarComponent } from '../parameter-group-search/parameter-group-search.component';

@Injectable({
  providedIn: 'root',
})
export class ParameterDragService {
  readonly format: string = 'applicaton/json';

  constructor(private parameterService: ParameterService) {}

  onDragStart(event: DragEvent, data: ParameterDragService.DragData) {
    event.dataTransfer?.setData(this.format, JSON.stringify(data));
  }

  dropInTrash(event: DragEvent) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.type === ParameterDragService.DataType.Group) {
        this.parameterService.removeParamGroup(data.value);
      } else if (
        data.type === ParameterDragService.DataType.Var &&
        data.srcUUID
      ) {
        this.parameterService
          .getParamGroup(data.srcUUID)
          ?.removeParam(data.value);
      }
    }
  }

  dropOnGroup(event: DragEvent, group: ParameterGroup) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.type === ParameterDragService.DataType.Var) {
        group.addParam(data.value);
      }
    }
  }

  dropOnSet(event: DragEvent, set: Parameters) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.type === ParameterDragService.DataType.Group) {
        set.addGroup(data.value);
      } else if (data.type === ParameterDragService.DataType.Var) {
        set.addParam(data.value);
      }
    }
  }

  onDragOver(event: DragEvent) {
    event.stopPropagation();
    event.preventDefault();
  }
}

export namespace ParameterDragService {
  export enum DataType {
    Var = 0,
    Group = 1,
    Set = 2,
    Callback = 3,
  }

  export interface DragData {
    type: DataType;
    value: string;
    srcUUID?: string;
  }
}

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

@Component({
  selector: 'type-col-header',
  template: `<col-header
    [value]="value"
    [classes]="classes"
    (drop)="parameterDragService.dropOnSet($event, row)"
    (dragover)="parameterDragService.onDragOver($event)"
  ></col-header>`,
})
export class TypeColHeaderComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: string = '';

  classes: string[] = [
    'type',
    'border-end-0',
    'rounded-0',
    'rounded-start',
    'text-end',
  ];

  constructor(protected parameterDragService: ParameterDragService) {}
}

@Component({
  selector: 'type-col-header',
  template: `<col-header
    [value]="value"
    [classes]="classes"
    (drop)="parameterDragService.dropOnSet($event, row)"
    (dragover)="parameterDragService.onDragOver($event)"
  ></col-header>`,
})
export class NameColHeaderComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: string = '';

  classes: string[] = ['border-start-0', 'rounded-0', 'rounded-end'];

  constructor(protected parameterDragService: ParameterDragService) {}
}

@Component({
  selector: 'app-group',
  templateUrl: './group-display.component.html',
  styleUrls: ['./group-display.component.css'],
})
export class GroupComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: Set<string> = new Set<string>();

  constructor(
    protected parameterService: ParameterService,
    protected parameterDragService: ParameterDragService
  ) {}
}

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
      component: VarComponent,
    }),
    new Column({
      key: 'groups',
      width: 1,
      cellClasses: ['border-start'],
      component: GroupComponent,
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
